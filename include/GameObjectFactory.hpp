#pragma once

#include "Collectible.hpp"
#include "Enemy.hpp"
#include "Interactable.hpp"
#include "MapLoader.hpp"

#include <memory>
#include <string>

class ProgressModel;

class GameObjectFactory
{
public:
    // Factory Method pattern: converts generic TMX objects
    // into concrete gameplay objects.
    static std::unique_ptr<Enemy> createEnemy(const std::string& levelId,
                                              const MapLoader::MapObject& object);
    static std::unique_ptr<Collectible> createCollectible(const std::string& levelId,
                                                          const MapLoader::MapObject& object,
                                                          const ProgressModel& progress);
    static std::unique_ptr<Interactable> createInteractable(const std::string& levelId,
                                                            const MapLoader::MapObject& object);
    static std::string makeObjectId(const std::string& levelId, const MapLoader::MapObject& object);
};
