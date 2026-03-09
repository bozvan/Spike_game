#include "Hedgehog.h"
#include <iostream>

Hedgehog::Hedgehog(sf::Vector2f position, sf::Vector2f size, sf::Color color)
    : isPressed(false)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(color);
}

void Hedgehog::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords({mousePressed->position.x, mousePressed->position.y});
            if (shape.getGlobalBounds().contains(mousePos))
            {
                isPressed = true;
                std::cout << "Кнопка нажата!" << std::endl;
            }
        }
    }
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        sf::Vector2f newPos = shape.getPosition();
        const float speed = 10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            newPos.y -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            newPos.x -= speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            newPos.x += speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            newPos.y += speed;
        shape.setPosition(newPos);
    }
}

void Hedgehog::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

bool Hedgehog::wasClicked() const
{
    return isPressed;
}
