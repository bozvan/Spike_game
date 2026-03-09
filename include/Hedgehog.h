#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SFML/Graphics.hpp>

class Hedgehog {
private:
    sf::RectangleShape shape;
    bool isPressed;

public:
    Hedgehog(sf::Vector2f position, sf::Vector2f size, sf::Color color);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    bool wasClicked() const;
};

#endif
