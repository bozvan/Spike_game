#include <SFML/Graphics.hpp>
#include "Hedgehog.h"
#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    setlocale(LC_ALL, "rus");

    std::string hedgehogTexturePath = "assets/textures/Hedgehog.png";
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Simple button");
    window.setFramerateLimit(60);


    fs::path p = hedgehogTexturePath;
    if (!fs::exists(p)) {
        std::cerr << "File don`t exist: " << fs::absolute(p) << '\n';
    } else {
        std::cout << "File exists. Size: " << fs::file_size(p) << " bite\n";
    }
    Hedgehog button(sf::Vector2f(300, 250), sf::Color(100, 100, 250), hedgehogTexturePath);

    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            button.handleEvent(*event, window);
        }
        button.update(deltaTime);

        window.clear(sf::Color::White);
        button.draw(window);
        window.display();
    }
    return 0;
}
