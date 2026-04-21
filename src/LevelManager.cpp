#include "LevelManager.hpp"

#include <iostream>

bool LevelManager::registerLevel(LevelDefinition definition)
{
    if (definition.id.empty() || definition.mapPath.empty())
    {
        return false;
    }

    m_levels[definition.id] = std::move(definition);
    return true;
}

bool LevelManager::loadLevel(const std::string& levelId, const std::string& spawnName)
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
    rebuildCaches();
    return true;
}

void LevelManager::update(const float deltaTimeSeconds)
{
    m_map.update(deltaTimeSeconds);
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

void LevelManager::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_map, states);
}

void LevelManager::rebuildCaches()
{
    m_collisionRects = collectRects(m_map.getObjects("Collisions"));
    m_ladderRects = collectRects(m_map.getObjects("Ladders"));
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
