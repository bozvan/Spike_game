#include "GameObjectFactory.hpp"

#include "PatrolStrategy.hpp"
#include "ProgressModel.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace
{
std::string toLowerCopy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char symbol)
        {
            return static_cast<char>(std::tolower(symbol));
        });

    return value;
}

std::string getProperty(const MapLoader::MapObject& object,
                        const std::string& propertyName,
                        const std::string& fallback = {})
{
    const auto it = object.properties.find(propertyName);
    return it == object.properties.end() ? fallback : it->second;
}

float getFloatProperty(const MapLoader::MapObject& object,
                       const std::string& propertyName,
                       const float fallback)
{
    const auto it = object.properties.find(propertyName);
    if (it == object.properties.end() || it->second.empty())
    {
        return fallback;
    }

    return std::strtof(it->second.c_str(), nullptr);
}

bool getBoolProperty(const MapLoader::MapObject& object,
                     const std::string& propertyName,
                     const bool fallback)
{
    const auto it = object.properties.find(propertyName);
    if (it == object.properties.end())
    {
        return fallback;
    }

    const std::string lowered = toLowerCopy(it->second);
    return lowered == "true" || lowered == "1" || lowered == "yes";
}

PartColor parsePartColor(const MapLoader::MapObject& object)
{
    const std::string lowered = toLowerCopy(getProperty(object, "color", "red"));
    if (lowered == "blue")
    {
        return PartColor::Blue;
    }

    if (lowered == "green")
    {
        return PartColor::Green;
    }

    return PartColor::Red;
}
}

std::unique_ptr<Enemy> GameObjectFactory::createEnemy(const std::string& levelId,
                                                      const MapLoader::MapObject& object)
{
    if (object.type == "BatEnemy" || object.type == "Bat" || object.type == "bat")
    {
        float patrolMinX = object.getPropertyFloat("patrolMinX", object.bounds.position.x - 150);
        float patrolMaxX = object.getPropertyFloat("patrolMaxX", object.bounds.position.x + 150);
        float speed = object.getPropertyFloat("speed", 100.0f);

        auto strategy = std::make_unique<HorizontalPatrolStrategy>(patrolMinX, patrolMaxX);

        return std::make_unique<BatEnemy>(
            object.name + "_" + std::to_string(object.id),
            object.bounds.getCenter(),
            sf::Vector2f(object.bounds.size.x, object.bounds.size.y),
            std::move(strategy),
            speed
        );
    }

    if (object.type == "Mushroom" || object.type == "mushroom" || object.type == "MushroomEnemy")
    {
        float patrolMinX = object.getPropertyFloat("patrolMinX", object.bounds.position.x - 100);
        float patrolMaxX = object.getPropertyFloat("patrolMaxX", object.bounds.position.x + 100);
        float speed = object.getPropertyFloat("speed", 40.0f);

        auto strategy = std::make_unique<HorizontalPatrolStrategy>(patrolMinX, patrolMaxX);

        return std::make_unique<MushroomEnemy>(
            object.name + "_" + std::to_string(object.id),
            object.bounds.getCenter(),
            sf::Vector2f(object.bounds.size.x, object.bounds.size.y),
            std::move(strategy),
            speed
        );
    }

    return nullptr;
}
}

std::unique_ptr<Collectible> GameObjectFactory::createCollectible(const std::string& levelId,
                                                                  const MapLoader::MapObject& object,
                                                                  const ProgressModel& progress)
{
    const std::string type = toLowerCopy(object.type);
    const std::string objectId = makeObjectId(levelId, object);

    if (progress.hasCollectedObject(objectId))
    {
        return nullptr;
    }

    if (type == "key")
    {
        return std::make_unique<KeyCollectible>(objectId, object.bounds);
    }

    if (type == "part")
    {
        return std::make_unique<PartCollectible>(objectId, object.bounds, parsePartColor(object));
    }

    return nullptr;
}

std::unique_ptr<Interactable> GameObjectFactory::createInteractable(const std::string& levelId,
                                                                    const MapLoader::MapObject& object)
{
    const std::string type = toLowerCopy(object.type);
    const std::string objectId = makeObjectId(levelId, object);

    if (type == "door")
    {
        return std::make_unique<DoorInteractable>(
            objectId,
            object.bounds,
            getProperty(object, "targetLevel", levelId),
            getProperty(object, "targetSpawn", "PlayerSpawn"),
            getBoolProperty(object, "requiresAllKeys", true));
    }

    if (type == "megadoor" || type == "goaldoor" || type == "finaldoor")
    {
        return std::make_unique<MegaDoorInteractable>(
            objectId,
            object.bounds,
            getProperty(object, "targetLevel", kVictoryLevelId),
            getProperty(object, "targetSpawn", "PlayerSpawn"));
    }

    if (type == "elevator" || type == "lift")
    {
        return std::make_unique<ElevatorInteractable>(
            objectId,
            object.bounds,
            getProperty(object, "targetLevel", levelId),
            getProperty(object, "targetSpawn", "PlayerSpawn"),
            static_cast<std::size_t>(std::max(0.f, getFloatProperty(object, "requiredKeys", 3.f))));
    }

    return nullptr;
}

std::string GameObjectFactory::makeObjectId(const std::string& levelId, const MapLoader::MapObject& object)
{
    if (!object.name.empty())
    {
        return levelId + "::" + object.name;
    }

    return levelId + "::" + object.type + "::" + std::to_string(object.id);
}
