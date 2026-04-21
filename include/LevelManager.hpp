#pragma once

#include "MapLoader.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <unordered_map>
#include <vector>

class LevelManager : public sf::Drawable
{
public:
    struct LevelDefinition
    {
        std::string id;
        std::string mapPath;
        std::string defaultSpawn{"PlayerSpawn"};
    };

    bool registerLevel(LevelDefinition definition);
    bool loadLevel(const std::string& levelId, const std::string& spawnName = "");
    void update(float deltaTimeSeconds);

    [[nodiscard]] const std::string& getCurrentLevelId() const;
    [[nodiscard]] const MapLoader& getMap() const;
    [[nodiscard]] const std::vector<sf::FloatRect>& getCollisionRects() const;
    [[nodiscard]] const std::vector<sf::FloatRect>& getLadderRects() const;
    [[nodiscard]] const std::vector<MapLoader::MapObject>& getObjects(const std::string& layerName) const;
    [[nodiscard]] sf::Vector2f getSpawnPosition(const std::string& spawnName = "") const;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void rebuildCaches();
    static std::vector<sf::FloatRect> collectRects(const std::vector<MapLoader::MapObject>& objects);

    MapLoader m_map;
    std::unordered_map<std::string, LevelDefinition> m_levels;
    std::string m_currentLevelId;
    std::string m_currentSpawnName;
    std::vector<sf::FloatRect> m_collisionRects;
    std::vector<sf::FloatRect> m_ladderRects;
};
