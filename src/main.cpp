#include <SFML/Graphics.hpp>

#include "Hedgehog.h"
#include "LevelManager.hpp"
#include "ProgressModel.hpp"
#include "Projectile.h"

#include <algorithm>
#include <optional>
#include <vector>

namespace
{
void updateCamera(sf::View& camera, const Hedgehog& player, const LevelManager& levelManager)
{
    const sf::Vector2f worldSize = levelManager.getMap().getWorldSize();
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
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({960, 540}), "Spike Game TMX Demo");
    window.setFramerateLimit(60);

    ProgressModel progress;
    LevelManager levelManager;
    levelManager.registerLevel({"demo", "assets/maps/demo_map.tmx", "PlayerSpawn"});

    if (!levelManager.loadLevel("demo", "PlayerSpawn", progress))
    {
        return 1;
    }

    Hedgehog player("assets/textures/Hedgehog.png", {64.f, 64.f});
    player.respawn(levelManager.getSpawnPosition());
    std::vector<Projectile> projectiles;

    sf::View camera = window.getDefaultView();
    camera.setSize({960.f, 540.f});
    updateCamera(camera, player, levelManager);

    sf::Clock clock;

    while (window.isOpen())
    {
        const float deltaTimeSeconds = clock.restart().asSeconds();
        bool interactRequested = false;

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
                    interactRequested = true;
                }
            }

            player.handleEvent(*event);
        }

        player.update(deltaTimeSeconds, levelManager.getCollisionRects(), levelManager.getLadderRects());

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

        levelManager.update(deltaTimeSeconds, player, progress, projectiles, interactRequested);

        if (const std::optional<LevelTransitionRequest> transition = levelManager.consumePendingTransition())
        {
            if (levelManager.loadLevel(transition->targetLevelId, transition->targetSpawnName, progress))
            {
                player.respawn(levelManager.getSpawnPosition());
                projectiles.clear();
            }
        }

        updateCamera(camera, player, levelManager);
        window.setView(camera);

        window.clear(sf::Color(150, 200, 255));
        window.draw(levelManager);
        for (const Projectile& projectile : projectiles)
        {
            projectile.draw(window);
        }
        player.draw(window);
        window.display();
    }

    return 0;
}
