#include "LevelManager.hpp"

#include "Collectible.hpp"
#include "Enemy.hpp"
#include "GameObjectFactory.hpp"
#include "Hedgehog.h"
#include "Interactable.hpp"
#include "ProgressModel.hpp"
#include "ProjectilePool.hpp"
#include "MushroomEnemy.hpp"

#include <algorithm>
#include <iostream>

namespace
{
std::size_t countKeysInEntities(const std::vector<MapLoader::MapObject>& objects)
{
    std::size_t keyCount = 0U;
    for (const MapLoader::MapObject& object : objects)
    {
        if (object.type == "Key" || object.type == "key")
        {
            ++keyCount;
        }
    }

    return keyCount;
}

std::size_t countKeysInLevelMap(const std::string& mapPath)
{
    MapLoader map;
    if (!map.loadFromFile(mapPath))
    {
        std::cerr << "Failed to preload level map for key count: " << mapPath << '\n';
        return 0U;
    }

    return countKeysInEntities(map.getObjects("Entities"));
}

void drawPlatform(sf::RenderTarget& target, const sf::FloatRect& bounds, const sf::RenderStates states)
{
    sf::RectangleShape body(bounds.size);
    body.setPosition(bounds.position);
    body.setFillColor(sf::Color(126, 88, 52));
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color(73, 46, 27));
    target.draw(body, states);

    const float topHeight = std::min(8.f, bounds.size.y);
    sf::RectangleShape grass({bounds.size.x, topHeight});
    grass.setPosition(bounds.position);
    grass.setFillColor(sf::Color(170, 132, 74));
    target.draw(grass, states);
}

void drawLadder(sf::RenderTarget& target, const sf::FloatRect& ladderRect, const sf::RenderStates states)
{
    sf::RectangleShape rail({4.f, ladderRect.size.y});
    rail.setFillColor(sf::Color(115, 75, 32));

    rail.setPosition({ladderRect.position.x + 5.f, ladderRect.position.y});
    target.draw(rail, states);

    rail.setPosition({ladderRect.position.x + ladderRect.size.x - 9.f, ladderRect.position.y});
    target.draw(rail, states);

    sf::RectangleShape rung({ladderRect.size.x - 12.f, 4.f});
    rung.setFillColor(sf::Color(181, 126, 54));

    for (float y = ladderRect.position.y + 10.f;
         y < ladderRect.position.y + ladderRect.size.y - 4.f;
         y += 16.f)
    {
        rung.setPosition({ladderRect.position.x + 6.f, y});
        target.draw(rung, states);
    }
}
}

LevelManager::~LevelManager() = default;

bool LevelManager::registerLevel(LevelDefinition definition)
{
    if (definition.id.empty() || definition.mapPath.empty())
    {
        return false;
    }

    m_levelKeyTargets[definition.id] = countKeysInLevelMap(definition.mapPath);
    m_levels[definition.id] = std::move(definition);
    return true;
}

void LevelManager::seedProgress(ProgressModel& progress) const
{
    for (const auto& [levelId, keyCount] : m_levelKeyTargets)
    {
        progress.setLevelKeyTarget(levelId, keyCount);
    }
}

bool LevelManager::loadLevel(const std::string& levelId,
                             const std::string& spawnName,
                             ProgressModel& progress)
{
    const auto it = m_levels.find(levelId);
    if (it == m_levels.end())
    {
        std::cerr << "Unknown level id: " << levelId << '\n';
        return false;
    }

    if (!m_map.loadFromFile(it->second.mapPath))
    {
        std::cerr << "Failed to load level map: " << it->second.mapPath << '\n';
        return false;
    }

    m_currentLevelId = levelId;
    m_currentSpawnName = spawnName.empty() ? it->second.defaultSpawn : spawnName;
    m_pendingTransition.reset();
    rebuildCaches();
    progress.setCurrentLevel(m_currentLevelId);
    rebuildGameplayObjects(progress);
    return true;
}

void LevelManager::update(const float deltaTimeSeconds,
                          Hedgehog& player,
                          ProgressModel& progress,
                          ProjectilePool& projectiles,
                          const bool interactRequested)
{
    m_map.update(deltaTimeSeconds);

    for (const std::unique_ptr<Enemy>& enemy : m_enemies)
    {
        enemy->update(deltaTimeSeconds);
    }

    for (Projectile& projectile : projectiles)
    {
        if (!projectile.isActive())
        {
            continue;
        }

        for (const std::unique_ptr<Enemy>& enemy : m_enemies)
        {
            if (enemy->tryHitByProjectile(projectile))
            {
                break;
            }
        }

        if (!projectile.isActive())
        {
            continue;
        }

        for (const sf::FloatRect& collider : m_collisionRects)
        {
            if (projectile.getBounds().findIntersection(collider).has_value())
            {
                projectile.deactivate();
                break;
            }
        }
    }

    for (const std::unique_ptr<Enemy>& enemy : m_enemies)
    {
        if (enemy->tryAttackPlayer(player, progress) && progress.getLives() <= 0)
        {
            std::cout << "Player lost all lives. Respawning at room start.\n";
            progress.restoreLives(3);
            player.respawn(getSpawnPosition());
            break;
        }
    }

    for (const std::unique_ptr<Collectible>& collectible : m_collectibles)
    {
        collectible->tryCollect(player.getBounds(), progress);
    }

    for (const std::unique_ptr<Interactable>& interactable : m_interactables)
    {
        interactable->syncState(player.getBounds(), progress);

        if (m_pendingTransition.has_value())
        {
            continue;
        }

        if (const auto transition = interactable->tryInteract(interactRequested, progress))
        {
            m_pendingTransition = transition;
        }
    }

    constrainPlayerToWorld(player);
}

const std::string& LevelManager::getCurrentLevelId() const
{
    return m_currentLevelId;
}

const MapLoader& LevelManager::getMap() const
{
    return m_map;
}

const std::vector<sf::FloatRect>& LevelManager::getCollisionRects() const
{
    return m_collisionRects;
}

const std::vector<sf::FloatRect>& LevelManager::getLadderRects() const
{
    return m_ladderRects;
}

const std::vector<MapLoader::MapObject>& LevelManager::getObjects(const std::string& layerName) const
{
    return m_map.getObjects(layerName);
}

sf::Vector2f LevelManager::getSpawnPosition(const std::string& spawnName) const
{
    const std::string& resolvedSpawn = spawnName.empty() ? m_currentSpawnName : spawnName;
    if (const MapLoader::MapObject* spawnObject = m_map.findObject("Spawns", resolvedSpawn))
    {
        return spawnObject->bounds.position;
    }

    return m_map.getPlayerSpawn();
}

std::optional<LevelTransitionRequest> LevelManager::consumePendingTransition()
{
    const auto result = m_pendingTransition;
    m_pendingTransition.reset();
    return result;
}

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_map, states);

    for (const sf::FloatRect& collider : m_collisionRects)
    {
        drawPlatform(target, collider, states);
    }

    for (const sf::FloatRect& ladder : m_ladderRects)
    {
        drawLadder(target, ladder, states);
    }

    for (const std::unique_ptr<Collectible>& collectible : m_collectibles)
    {
        target.draw(*collectible, states);
    }

    for (const std::unique_ptr<Interactable>& interactable : m_interactables)
    {
        target.draw(*interactable, states);
    }

    for (const std::unique_ptr<Enemy>& enemy : m_enemies)
    {
        target.draw(*enemy, states);
    }
}

void LevelManager::constrainPlayerToWorld(Hedgehog& player)
{
    const sf::Vector2f worldSize = m_map.getWorldSize();
    const sf::FloatRect bounds = player.getBounds();

    if (bounds.position.y > worldSize.y + 96.f)
    {
        std::cout << "Player left the level bounds. Respawning.\n";
        player.respawn(getSpawnPosition());
        return;
    }

    sf::Vector2f correctedPosition = bounds.position;
    bool corrected = false;

    if (correctedPosition.x < 0.f)
    {
        correctedPosition.x = 0.f;
        corrected = true;
    }

    const float maxX = std::max(0.f, worldSize.x - bounds.size.x);
    if (correctedPosition.x > maxX)
    {
        correctedPosition.x = maxX;
        corrected = true;
    }

    if (correctedPosition.y < -bounds.size.y)
    {
        correctedPosition.y = -bounds.size.y;
        corrected = true;
    }

    if (corrected)
    {
        player.setPosition(correctedPosition);
    }
}

void LevelManager::rebuildCaches()
{
    m_collisionRects = collectRects(m_map.getObjects("Collisions"));
    m_ladderRects = collectRects(m_map.getObjects("Ladders"));
}

void LevelManager::rebuildGameplayObjects(ProgressModel& progress)
{
    m_enemies.clear();
    m_collectibles.clear();
    m_interactables.clear();

    const auto& entities = m_map.getObjects("Entities");
    const std::size_t keyCount = countKeysInEntities(entities);
    progress.setLevelKeyTarget(m_currentLevelId, keyCount);

    for (const MapLoader::MapObject& object : entities)
    {
        if (std::unique_ptr<Enemy> enemy = GameObjectFactory::createEnemy(m_currentLevelId, object))
        {
            m_enemies.push_back(std::move(enemy));
            continue;
        }

        if (std::unique_ptr<Collectible> collectible =
                GameObjectFactory::createCollectible(m_currentLevelId, object, progress))
        {
            m_collectibles.push_back(std::move(collectible));
            continue;
        }

        if (std::unique_ptr<Interactable> interactable =
                GameObjectFactory::createInteractable(m_currentLevelId, object))
        {
            m_interactables.push_back(std::move(interactable));
        }
    }
}

std::vector<sf::FloatRect> LevelManager::collectRects(const std::vector<MapLoader::MapObject>& objects)
{
    std::vector<sf::FloatRect> rects;
    rects.reserve(objects.size());

    for (const MapLoader::MapObject& object : objects)
    {
        rects.push_back(object.bounds);
    }

    return rects;
}
