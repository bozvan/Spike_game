#include <SFML/Graphics.hpp>

#include "Hedgehog.h"
#include "MapLoader.hpp"
#include "Projectile.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{
struct GameProgress
{
    std::unordered_set<int> collectedKeys;
    bool doorOpened{};
    bool interactRequested{};
    std::size_t totalKeys{};
};

std::vector<sf::FloatRect> collectRects(const MapLoader& map, const std::string& layerName)
{
    std::vector<sf::FloatRect> rects;
    const auto& objects = map.getObjects(layerName);
    rects.reserve(objects.size());

    for (const MapLoader::MapObject& object : objects)
    {
        rects.push_back(object.bounds);
    }

    return rects;
}

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

bool isType(const MapLoader::MapObject& object, const std::string& expectedType)
{
    return toLowerCopy(object.type) == expectedType;
}

std::size_t countKeys(const std::vector<MapLoader::MapObject>& entities)
{
    return static_cast<std::size_t>(std::count_if(
        entities.begin(),
        entities.end(),
        [](const MapLoader::MapObject& entity)
        {
            return isType(entity, "key");
        }));
}

void updateCamera(sf::View& camera, const Hedgehog& player, const MapLoader& map)
{
    const sf::Vector2f worldSize = map.getWorldSize();
    const sf::Vector2f viewSize = camera.getSize();

    sf::Vector2f center = player.getCenter();

    if (worldSize.x <= viewSize.x)
    {
        center.x = worldSize.x * 0.5f;
    }
    else
    {
        center.x = std::clamp(center.x, viewSize.x * 0.5f, worldSize.x - viewSize.x * 0.5f);
    }

    if (worldSize.y <= viewSize.y)
    {
        center.y = worldSize.y * 0.5f;
    }
    else
    {
        center.y = std::clamp(center.y, viewSize.y * 0.5f, worldSize.y - viewSize.y * 0.5f);
    }

    camera.setCenter(center);
}

void collectKeys(const Hedgehog& player,
                 const std::vector<MapLoader::MapObject>& entities,
                 GameProgress& progress)
{
    for (const MapLoader::MapObject& entity : entities)
    {
        if (!isType(entity, "key"))
        {
            continue;
        }

        if (progress.collectedKeys.find(entity.id) != progress.collectedKeys.end())
        {
            continue;
        }

        if (!player.getBounds().findIntersection(entity.bounds).has_value())
        {
            continue;
        }

        progress.collectedKeys.insert(entity.id);
        std::cout << "Picked up key (" << progress.collectedKeys.size() << '/' << progress.totalKeys << ")\n";
    }
}

void handleDoorInteraction(const Hedgehog& player,
                           const std::vector<MapLoader::MapObject>& entities,
                           GameProgress& progress)
{
    if (!progress.interactRequested)
    {
        return;
    }

    progress.interactRequested = false;

    for (const MapLoader::MapObject& entity : entities)
    {
        if (!isType(entity, "door"))
        {
            continue;
        }

        if (!player.getBounds().findIntersection(entity.bounds).has_value())
        {
            continue;
        }

        if (!progress.doorOpened)
        {
            if (progress.collectedKeys.size() == progress.totalKeys)
            {
                progress.doorOpened = true;
                std::cout << "Door opened\n";
            }
            else
            {
                std::cout << "Door is locked. Keys: "
                          << progress.collectedKeys.size()
                          << '/'
                          << progress.totalKeys
                          << '\n';
            }
        }
        else
        {
            std::cout << "Entered door\n";
        }

        return;
    }
}

void drawLadder(sf::RenderTarget& target, const sf::FloatRect& ladderRect)
{
    sf::RectangleShape rail({4.f, ladderRect.size.y});
    rail.setFillColor(sf::Color(115, 75, 32));

    rail.setPosition({ladderRect.position.x + 5.f, ladderRect.position.y});
    target.draw(rail);

    rail.setPosition({ladderRect.position.x + ladderRect.size.x - 9.f, ladderRect.position.y});
    target.draw(rail);

    sf::RectangleShape rung({ladderRect.size.x - 12.f, 4.f});
    rung.setFillColor(sf::Color(181, 126, 54));

    for (float y = ladderRect.position.y + 10.f;
         y < ladderRect.position.y + ladderRect.size.y - 4.f;
         y += 16.f)
    {
        rung.setPosition({ladderRect.position.x + 6.f, y});
        target.draw(rung);
    }
}

void drawKey(sf::RenderTarget& target, const sf::FloatRect& bounds, const float timeSeconds)
{
    const float bob = std::sin(timeSeconds * 5.f + bounds.position.x * 0.01f) * 3.f;

    sf::CircleShape ring(7.f);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(4.f);
    ring.setOutlineColor(sf::Color(245, 210, 70));
    ring.setPosition({bounds.position.x + 4.f, bounds.position.y + 6.f + bob});
    target.draw(ring);

    sf::RectangleShape stem({14.f, 4.f});
    stem.setFillColor(sf::Color(245, 210, 70));
    stem.setPosition({bounds.position.x + 18.f, bounds.position.y + 14.f + bob});
    target.draw(stem);

    sf::RectangleShape tooth({4.f, 8.f});
    tooth.setFillColor(sf::Color(245, 210, 70));
    tooth.setPosition({bounds.position.x + 26.f, bounds.position.y + 14.f + bob});
    target.draw(tooth);

    tooth.setPosition({bounds.position.x + 30.f, bounds.position.y + 10.f + bob});
    target.draw(tooth);
}

void drawDoor(sf::RenderTarget& target,
              const sf::FloatRect& bounds,
              const bool isOpen,
              const bool playerNearby)
{
    sf::RectangleShape door(bounds.size);
    door.setPosition(bounds.position);
    door.setFillColor(isOpen ? sf::Color(96, 174, 89) : sf::Color(104, 62, 32));
    door.setOutlineThickness(3.f);
    door.setOutlineColor(playerNearby ? sf::Color(255, 231, 122) : sf::Color(50, 30, 18));
    target.draw(door);

    sf::CircleShape knob(4.f);
    knob.setFillColor(sf::Color(235, 192, 61));
    knob.setPosition({
        bounds.position.x + bounds.size.x - 14.f,
        bounds.position.y + bounds.size.y * 0.5f});
    target.draw(knob);

    if (!isOpen)
    {
        sf::RectangleShape plank({bounds.size.x - 12.f, 6.f});
        plank.setFillColor(sf::Color(72, 42, 20));
        plank.setPosition({bounds.position.x + 6.f, bounds.position.y + 18.f});
        target.draw(plank);
    }
}

void drawObjects(sf::RenderTarget& target,
                 const std::vector<MapLoader::MapObject>& ladders,
                 const std::vector<MapLoader::MapObject>& entities,
                 const Hedgehog& player,
                 const GameProgress& progress,
                 const float timeSeconds)
{
    for (const MapLoader::MapObject& ladder : ladders)
    {
        drawLadder(target, ladder.bounds);
    }

    for (const MapLoader::MapObject& entity : entities)
    {
        if (isType(entity, "key"))
        {
            if (progress.collectedKeys.find(entity.id) == progress.collectedKeys.end())
            {
                drawKey(target, entity.bounds, timeSeconds);
            }
        }
        else if (isType(entity, "door"))
        {
            const bool playerNearby = player.getBounds().findIntersection(entity.bounds).has_value();
            drawDoor(target, entity.bounds, progress.doorOpened, playerNearby);
        }
    }
}
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({960, 540}), "Spike Game TMX Demo");
    window.setFramerateLimit(60);

    MapLoader map;
    if (!map.loadFromFile("assets/maps/demo_map.tmx"))
    {
        return 1;
    }

    const auto& ladders = map.getObjects("Ladders");
    const auto& entities = map.getObjects("Entities");
    const std::vector<sf::FloatRect> collisionRects = collectRects(map, "Collisions");
    const std::vector<sf::FloatRect> ladderRects = collectRects(map, "Ladders");

    Hedgehog player("assets/textures/Hedgehog.png", {64.f, 64.f});
    player.setPosition(map.getPlayerSpawn());
    std::vector<Projectile> projectiles;

    GameProgress progress;
    progress.totalKeys = countKeys(entities);

    sf::View camera = window.getDefaultView();
    camera.setSize({960.f, 540.f});
    updateCamera(camera, player, map);

    sf::Clock clock;
    float elapsedTimeSeconds = 0.f;

    while (window.isOpen())
    {
        const float deltaTimeSeconds = clock.restart().asSeconds();
        elapsedTimeSeconds += deltaTimeSeconds;

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                camera.setSize({
                    static_cast<float>(resized->size.x),
                    static_cast<float>(resized->size.y)});
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::E)
                {
                    progress.interactRequested = true;
                }
            }

            player.handleEvent(*event);
        }

        map.update(deltaTimeSeconds);
        player.update(deltaTimeSeconds, collisionRects, ladderRects);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F))
        {
            player.shoot(projectiles);
        }

        for (Projectile& projectile : projectiles)
        {
            projectile.update(deltaTimeSeconds);
        }

        projectiles.erase(
            std::remove_if(
                projectiles.begin(),
                projectiles.end(),
                [](const Projectile& projectile)
                {
                    return !projectile.isActive();
                }),
            projectiles.end());

        collectKeys(player, entities, progress);
        handleDoorInteraction(player, entities, progress);

        updateCamera(camera, player, map);
        window.setView(camera);

        window.clear(sf::Color(150, 200, 255));
        window.draw(map);
        drawObjects(window, ladders, entities, player, progress, elapsedTimeSeconds);
        for (const Projectile& projectile : projectiles)
        {
            projectile.draw(window);
        }
        player.draw(window);
        window.display();
    }

    return 0;
}
