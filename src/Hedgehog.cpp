#include "Hedgehog.h"
#include <iostream>

Hedgehog::Hedgehog(sf::Vector2f position, sf::Vector2f size, sf::Color color)
    : isPressed(false)
{
    shape.setPosition(position);
    shape.setSize(size);
    shape.setFillColor(color);

    movement.x = 0.f;
    movement.y = 0.f;
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
                std::cout << "Button is pressed!" << std::endl;
            }
        }
    }
}

void Hedgehog::draw(sf::RenderWindow& window)
{
    window.draw(shape);
}

void Hedgehog::update(sf::Time &deltaTime)
{
    movement.x = 0.f;
    movement.y = 0.f;
    const float speed = 200.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        movement.y -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        movement.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        movement.x += speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        movement.y += speed;

    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length != 0)
        movement /= length;

    shape.move(movement * speed * deltaTime.asSeconds());
}

bool Hedgehog::wasClicked() const
{
    return isPressed;
}
