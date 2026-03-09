#include "SimpleButton.h"
#include <iostream>

SimpleButton::SimpleButton(sf::Vector2f position, sf::Vector2f size, sf::Color color)
    : isPressed(false)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(color);
}

void SimpleButton::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(
                {mousePressed->position.x, mousePressed->position.y}
                );
            if (shape.getGlobalBounds().contains(mousePos)) {
                isPressed = true;
                std::cout << "Кнопка нажата!" << std::endl;
            }
        }
    }
}

void SimpleButton::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

bool SimpleButton::wasClicked() const {
    return isPressed;
}
