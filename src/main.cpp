#include <SFML/Graphics.hpp>
#include "Hedgehog.h"
#include "Projectile.h"

#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

std::vector<std::vector<bool>> createAlphaMask(const sf::Image& image)
{
    auto size = image.getSize();

    std::vector<std::vector<bool>> mask(
        size.y,
        std::vector<bool>(size.x, false)
        );

    for (unsigned y = 0; y < size.y; y++)
    {
        for (unsigned x = 0; x < size.x; x++)
        {
            mask[y][x] = image.getPixel({x, y}).a > 0;
        }
    }

    return mask;
}

int main()
{
    setlocale(LC_ALL, "rus");

    std::string hedgehogTexturePath = "assets/textures/Hedgehog.png";
    std::vector<Projectile> projectiles;

    sf::Clock clock;

    sf::RenderWindow window(
        sf::VideoMode({1600, 600}),
        "Pixel Perfect Collision"
        );

    window.setFramerateLimit(60);

    // Проверка файла
    fs::path testPath = "assets/textures/collision_test.png";

    if (!fs::exists(testPath))
    {
        std::cerr << "Test texture not found!\n";
    }
    else
    {
        std::cout << "Test texture exists. Size: "
                  << fs::file_size(testPath)
                  << " bytes\n";
    }

    // ---------- Hedgehog ----------
    Hedgehog hedgehog(
        sf::Vector2f(300, 150),
        hedgehogTexturePath
        );

    // ---------- Test object ----------
    sf::Texture test_texture;
    test_texture.loadFromFile("assets/textures/collision_test.png");

    sf::Sprite test_sprite(test_texture);

    int frameWidth = test_texture.getSize().x;
    int frameHeight = test_texture.getSize().y;

    test_sprite.setTextureRect(
        sf::IntRect({0,0},{frameWidth,frameHeight})
        );

    test_sprite.setPosition({450,450});
    test_sprite.setOrigin({
        test_sprite.getLocalBounds().getCenter().x,
        0.f
    });

    // ---------- Image + Alpha mask ----------
    sf::Image test_image;
    test_image.loadFromFile("assets/textures/collision_test.png");

    auto testMask = createAlphaMask(test_image);

    // ---------- Game loop ----------
    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            hedgehog.handleEvent(*event, window);

        }

        window.clear(sf::Color::White);

        // Update
        hedgehog.update(
            deltaTime,
            projectiles,
            test_sprite,
            testMask
            );

        for (auto& projectile : projectiles)
            projectile.update(deltaTime.asSeconds());

        // Draw projectiles
        std::for_each(
            projectiles.begin(),
            projectiles.end(),
            [&window](Projectile& p)
            {
                p.draw(window);
            }
            );

        window.draw(test_sprite);
        hedgehog.draw(window);

        // Remove inactive projectiles
        projectiles.erase(
            std::remove_if(
                projectiles.begin(),
                projectiles.end(),
                [](const Projectile& p)
                {
                    return !p.isActive();
                }
                ),
            projectiles.end()
            );

        window.display();
    }

    return 0;
}
