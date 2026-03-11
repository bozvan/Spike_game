#include <SFML/Graphics.hpp>
#include "Hedgehog.h"
#include "Projectile.h"
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

int main() {
    setlocale(LC_ALL, "rus");

    std::string hedgehogTexturePath = "assets/textures/Hedgehog.png";
    std::vector<Projectile> projectiles;

    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Simple button");
    window.setFramerateLimit(60);


    fs::path p = hedgehogTexturePath;
    if (!fs::exists(p)) {
        std::cerr << "File don`t exist: " << fs::absolute(p) << '\n';
    } else {
        std::cout << "File exists. Size: " << fs::file_size(p) << " bite\n";
    }

    fs::path bulletPath = "assets/textures/bullet-mushroom.png";
    if (!fs::exists(bulletPath)) {
        std::cerr << "Bullet texture not found!\n";
    } else {
        std::cout << "Bullet texture exists. Size: " << fs::file_size(bulletPath) << " bytes\n";
    }

    Hedgehog hedgehog(sf::Vector2f(300, 250), hedgehogTexturePath);

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            hedgehog.handleEvent(*event, window);
        }
        window.clear(sf::Color::White);

        //Update and draw
        hedgehog.update(deltaTime, projectiles);
        for (auto& projectile : projectiles) {
            projectile.update(deltaTime.asSeconds());
        }
        std::for_each(projectiles.begin(), projectiles.end(),
                      [&window](Projectile &p)
                    {
                          p.draw(window);
                    });
        hedgehog.draw(window);


        //Removing inactive projectiles
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(),
                           [](const Projectile& p) { return !p.isActive(); }),
            projectiles.end()
            );

        window.display();
    }
    return 0;
}
