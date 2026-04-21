#include <SFML/Graphics.hpp>

#include "Hedgehog.h"
#include "HudOverlay.hpp"
#include "LevelManager.hpp"
#include "ProgressModel.hpp"
#include "Projectile.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <vector>

namespace
{
constexpr const char* kBaseWindowTitle = "Spike Game TMX Demo";
constexpr const char* kVictoryWindowTitle = "Spike Game - Victory! All keys and parts collected";

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

void drawVictoryOverlay(sf::RenderTarget& target)
{
    const sf::View& view = target.getView();
    const sf::Vector2f viewSize = view.getSize();
    const sf::Vector2f topLeft = view.getCenter() - viewSize * 0.5f;

    sf::RectangleShape veil(viewSize);
    veil.setPosition(topLeft);
    veil.setFillColor(sf::Color(10, 14, 26, 190));
    target.draw(veil);

    sf::RectangleShape panel({420.f, 240.f});
    panel.setOrigin(panel.getSize() * 0.5f);
    panel.setPosition(view.getCenter());
    panel.setFillColor(sf::Color(33, 41, 62, 230));
    panel.setOutlineThickness(4.f);
    panel.setOutlineColor(sf::Color(255, 219, 102, 220));
    target.draw(panel);

    sf::RectangleShape door({132.f, 160.f});
    door.setOrigin(door.getSize() * 0.5f);
    door.setPosition({view.getCenter().x, view.getCenter().y - 12.f});
    door.setFillColor(sf::Color(240, 189, 58));
    door.setOutlineThickness(5.f);
    door.setOutlineColor(sf::Color(255, 241, 186));
    target.draw(door);

    sf::RectangleShape inset({98.f, 126.f});
    inset.setOrigin(inset.getSize() * 0.5f);
    inset.setPosition(door.getPosition());
    inset.setFillColor(sf::Color(255, 219, 116));
    target.draw(inset);

    sf::CircleShape core(13.f, 8);
    core.setOrigin({13.f, 13.f});
    core.setPosition({door.getPosition().x, door.getPosition().y + 8.f});
    core.setFillColor(sf::Color(255, 250, 228));
    core.setOutlineThickness(3.f);
    core.setOutlineColor(sf::Color(168, 118, 27));
    target.draw(core);

    const std::array<sf::Color, 3> gemColors{
        sf::Color(220, 71, 71),
        sf::Color(68, 139, 255),
        sf::Color(76, 194, 104)};
    for (std::size_t index = 0; index < gemColors.size(); ++index)
    {
        sf::ConvexShape gem;
        gem.setPointCount(4);
        gem.setPoint(0, {0.f, -12.f});
        gem.setPoint(1, {12.f, 0.f});
        gem.setPoint(2, {0.f, 12.f});
        gem.setPoint(3, {-12.f, 0.f});
        gem.setFillColor(gemColors[index]);
        gem.setOutlineThickness(2.f);
        gem.setOutlineColor(sf::Color(255, 255, 255, 180));
        gem.setPosition({
            view.getCenter().x - 56.f + static_cast<float>(index) * 56.f,
            view.getCenter().y + 92.f});
        target.draw(gem);
    }

    for (int index = 0; index < 6; ++index)
    {
        const float offsetX = -120.f + static_cast<float>(index) * 40.f;
        sf::CircleShape ring(5.f);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(2.f);
        ring.setOutlineColor(sf::Color(255, 223, 93));
        ring.setPosition({
            view.getCenter().x + offsetX - 18.f,
            view.getCenter().y - 102.f + (index % 2 == 0 ? 0.f : 10.f)});
        target.draw(ring);

        sf::RectangleShape stem({10.f, 3.f});
        stem.setFillColor(sf::Color(255, 223, 93));
        stem.setPosition({ring.getPosition().x + 11.f, ring.getPosition().y + 5.f});
        target.draw(stem);
    }
}
}

int main()
{
    constexpr sf::Vector2u windowSize{1280U, 720U};

    sf::RenderWindow window(sf::VideoMode(windowSize), kBaseWindowTitle);
    window.setFramerateLimit(60);

    ProgressModel progress;
    HudOverlay hud;
    progress.addObserver(hud);
    LevelManager levelManager;
    levelManager.registerLevel({"demo", "assets/maps/demo_map.tmx", "PlayerSpawn"});
    levelManager.registerLevel({"room_tower", "assets/maps/room_tower.tmx", "PlayerSpawn"});
    levelManager.registerLevel({"room_caves", "assets/maps/room_caves.tmx", "PlayerSpawn"});
    levelManager.seedProgress(progress);

    if (!levelManager.loadLevel("demo", "PlayerSpawn", progress))
    {
        return 1;
    }

    Hedgehog player("assets/textures/Hedgehog.png", {64.f, 64.f});
    player.respawn(levelManager.getSpawnPosition());
    std::vector<Projectile> projectiles;
    bool victoryAchieved = false;

    std::cout << "Objective: collect every key and part, then return to the golden mega door in the first room.\n";

    sf::View camera = window.getDefaultView();
    camera.setSize(sf::Vector2f(windowSize));
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

            if (!victoryAchieved)
            {
                player.handleEvent(*event);
            }
        }

        if (!victoryAchieved)
        {
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
                if (transition->targetLevelId == kVictoryLevelId)
                {
                    victoryAchieved = true;
                    projectiles.clear();
                    window.setTitle(kVictoryWindowTitle);
                    std::cout << "Victory! All keys and parts collected.\n";
                }
                else if (levelManager.loadLevel(transition->targetLevelId, transition->targetSpawnName, progress))
                {
                    player.respawn(levelManager.getSpawnPosition());
                    projectiles.clear();
                }
            }

            updateCamera(camera, player, levelManager);
            window.setView(camera);
        }

        window.clear(sf::Color(150, 200, 255));
        window.draw(levelManager);
        for (const Projectile& projectile : projectiles)
        {
            projectile.draw(window);
        }
        player.draw(window);
        window.setView(window.getDefaultView());
        window.draw(hud);
        if (victoryAchieved)
        {
            drawVictoryOverlay(window);
        }
        window.display();
    }

    return 0;
}
