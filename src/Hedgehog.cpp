#include "Hedgehog.h"
#include <iostream>


Hedgehog::Hedgehog(sf::Vector2f position, sf::Vector2f size, sf::Color color)
    : isPressed(false),
      jumping(0), jump_force(250), gravity(1000), speed(0, 0), speedRun(200.f)
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

void Hedgehog::stop_falling(float y)
{
    if (jumping)
    {
        jumping = false;
        speed.y = 0;
        shape.setPosition({shape.getPosition().x, y});
    }
    std::cout << "current position y = " << shape.getPosition().y << "\n";
}

void Hedgehog::jump(Direction direction=Direction::DEFAULT)
{
    bool falling = false;

    std::cout << "stopFallingPosition = " << stopFallingPosition << "\n";
    if (!jumping)
    {
        speed.x = 0;
        if (direction == Direction::LEFT)
        {
            speed.x -= speedRun;
        }
        else if (direction == Direction::RIGHT)
        {
            speed.x += speedRun;
        }
        if (!falling)
        {
            stopFallingPosition = shape.getPosition().y;
            speed.y = -jump_force;
        }
        jumping = true;
    }
}

void Hedgehog::set_position(const sf::Vector2f& pos)
{
    shape.setPosition(pos);
}

void Hedgehog::update(sf::Time &deltaTime)
{
    movement.x = 0.f;
    movement.y = 0.f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        jump(Direction::LEFT);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) &&
          sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        jump(Direction::RIGHT);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        jump();

    if (shape.getPosition().y > stopFallingPosition)
        stop_falling(stopFallingPosition);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        movement.y -= speedRun;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
        movement.x -= speedRun;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        movement.x += speedRun;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        movement.y += speedRun;

    float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length != 0)
        movement /= length;

    if (jumping)
    {
        speed.y += gravity * deltaTime.asSeconds();
        shape.move(speed * deltaTime.asSeconds());
    }
    else
    {
        shape.move(movement * speedRun * deltaTime.asSeconds());
    }
}

bool Hedgehog::wasClicked() const
{
    return isPressed;
}
