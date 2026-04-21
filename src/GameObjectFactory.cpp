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
    const std::string type = toLowerCopy(object.type);
    if (type != "bat")
    {
        return nullptr;
    }

    const std::string objectId = makeObjectId(levelId, object);
    const sf::Vector2f size = {
        object.bounds.size.x > 0.f ? object.bounds.size.x : 48.f,
        object.bounds.size.y > 0.f ? object.bounds.size.y : 28.f};
    const sf::Vector2f center = object.bounds.getCenter();

    const float patrolMinX = getFloatProperty(object, "patrolMinX", center.x - 96.f);
    const float patrolMaxX = getFloatProperty(object, "patrolMaxX", center.x + 96.f);
    const float speed = getFloatProperty(object, "speed", 90.f);

    return std::make_unique<BatEnemy>(
        objectId,
        center,
        size,
        std::make_unique<HorizontalPatrolStrategy>(patrolMinX, patrolMaxX, speed));
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

    if (type == "elevator" || type == "lift")
    {
        return std::make_unique<ElevatorInteractable>(
            objectId,
            object.bounds,
            getProperty(object, "targetLevel", levelId),
            getProperty(object, "targetSpawn", "PlayerSpawn"));
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
