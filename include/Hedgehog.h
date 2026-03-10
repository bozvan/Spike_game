#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SFML/Graphics.hpp>

class Hedgehog {
private:
    sf::RectangleShape shape;
    sf::Vector2f movement;
    bool isPressed;

public:
    Hedgehog(sf::Vector2f position, sf::Vector2f size, sf::Color color);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void update(sf::Time &deltaTime);
    bool wasClicked() const;
};

#endif
