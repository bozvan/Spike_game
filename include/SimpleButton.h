#ifndef SIMPLEBUTTON_H
#define SIMPLEBUTTON_H

#include <SFML/Graphics.hpp>

class SimpleButton {
private:
    sf::RectangleShape shape;
    bool isPressed;

public:
    SimpleButton(sf::Vector2f position, sf::Vector2f size, sf::Color color);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    bool wasClicked() const;
};

#endif
