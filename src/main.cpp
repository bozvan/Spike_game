#include <SFML/Graphics.hpp>
#include "Hedgehog.h"

int main() {
    setlocale(LC_ALL, "rus");
    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Simple button");
    window.setFramerateLimit(120);

    Hedgehog button(sf::Vector2f(300, 250), sf::Vector2f(100, 100), sf::Color(100, 100, 250));

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
