#pragma once

#include "Collectible.hpp"
#include "Enemy.hpp"
#include "Interactable.hpp"
#include "MapLoader.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Hedgehog;
class ProgressModel;
class ProjectilePool;

class LevelManager : public sf::Drawable
{
public:
    struct LevelDefinition
    {
        std::string id;
        std::string mapPath;
        std::string defaultSpawn{"PlayerSpawn"};
    };

    ~LevelManager();

    bool registerLevel(LevelDefinition definition);
    void seedProgress(ProgressModel& progress) const;
    bool loadLevel(const std::string& levelId, const std::string& spawnName, ProgressModel& progress);
    void update(float deltaTimeSeconds,
                Hedgehog& player,
                ProgressModel& progress,
                ProjectilePool& projectiles,
                bool interactRequested);

    [[nodiscard]] const std::string& getCurrentLevelId() const;
    [[nodiscard]] const MapLoader& getMap() const;
    [[nodiscard]] const std::vector<sf::FloatRect>& getCollisionRects() const;
    [[nodiscard]] const std::vector<sf::FloatRect>& getLadderRects() const;
    [[nodiscard]] const std::vector<MapLoader::MapObject>& getObjects(const std::string& layerName) const;
    [[nodiscard]] sf::Vector2f getSpawnPosition(const std::string& spawnName = "") const;
    [[nodiscard]] std::optional<LevelTransitionRequest> consumePendingTransition();

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void constrainPlayerToWorld(Hedgehog& player);
    void rebuildCaches();
    void rebuildGameplayObjects(ProgressModel& progress);
    static std::vector<sf::FloatRect> collectRects(const std::vector<MapLoader::MapObject>& objects);

    MapLoader m_map;
    std::unordered_map<std::string, LevelDefinition> m_levels;
    std::unordered_map<std::string, std::size_t> m_levelKeyTargets;
    std::string m_currentLevelId;
    std::string m_currentSpawnName;
    std::vector<sf::FloatRect> m_collisionRects;
    std::vector<sf::FloatRect> m_ladderRects;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Collectible>> m_collectibles;
    std::vector<std::unique_ptr<Interactable>> m_interactables;
    std::optional<LevelTransitionRequest> m_pendingTransition;
};
