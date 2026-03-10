#include "Hedgehog.h"
#include <iostream>
#include <string>
#include <cmath>

Hedgehog::Hedgehog(sf::Vector2f position, sf::Color color, std::string &texturePath)
    : texture(), sprite(texture), isPressed(false),
    jumping(0), jump_force(250), gravity(1000), speed(0, 0), speedRun(400.f), state(0)
{
    if (!texture.loadFromFile(texturePath))
    {
        std::cout << "Texture Hedgehog is not found!\n";
        std::cout << "Hedgehog Path is " << texturePath << "\n";
        shape.setFillColor(color);
    };

    int frameWidth = texture.getSize().x;
    int frameHeight = texture.getSize().y/2; // two frames

    //sprite.setSize(sf::Vector2f(frameWidth, frameHeight));
    sprite.setPosition(position);
    sprite.setTexture(texture);

    sprite.setTextureRect(sf::IntRect({0, 0}, {frameWidth, frameHeight}));
    sprite.setOrigin({sprite.getLocalBounds().getCenter().x, 0.f});

    movement.x = 0.f;
    movement.y = 0.f;
}

sf::Sprite& Hedgehog::getSprite() { return sprite; }
const sf::Sprite& Hedgehog::getSprite() const { return sprite; }


void Hedgehog::handleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mousePressed->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos = window.mapPixelToCoords({mousePressed->position.x, mousePressed->position.y});
            if (sprite.getGlobalBounds().contains(mousePos))
            {
                isPressed = true;
                std::cout << "Button is pressed!" << std::endl;
            }
        }
    }
}

void Hedgehog::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

void Hedgehog::stop_falling(float y)
{
    if (jumping)
    {
        jumping = false;
        speed.y = 0;
        sprite.setPosition({sprite.getPosition().x, y});
    }
    //std::cout << "current position y = " << shape.getPosition().y << "\n";
}

void Hedgehog::jump(Direction direction=Direction::DEFAULT)
{
    bool falling = false;

    //std::cout << "stopFallingPosition = " << stopFallingPosition << "\n";
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
            stopFallingPosition = sprite.getPosition().y;
            speed.y = -jump_force;
        }
        jumping = true;
    }
}

void Hedgehog::set_position(const sf::Vector2f& pos)
{
    sprite.setPosition(pos);
}

void Hedgehog::run()
{
    running = true;
    //std::cout << "change pic! state = " << state << "\n";
    int frameWidth = texture.getSize().x;
    int frameHeight = texture.getSize().y/2;
    int frameY = frameHeight;
    int newFrameHeight = texture.getSize().y;

    state = (state + 1) % 2; // 2 frames
    switch (state)
    {
    case 0:  sprite.setTextureRect(sf::IntRect({0, frameY}, {frameWidth, newFrameHeight})); break;
    case 1:  sprite.setTextureRect(sf::IntRect({0, 0}, {frameWidth, frameHeight})); break;
    }


}

void Hedgehog::update(sf::Time &deltaTime)
{
    movement.x = 0.f;
    movement.y = 0.f;

    int frameWidth = texture.getSize().x;
    int frameHeight = texture.getSize().y/2; // two frames

    running = false;
    if (!running)
    {
        //std::cout << "stop pic \n";
        sprite.setTextureRect(sf::IntRect({0, 0}, {frameWidth, frameHeight}));
    }

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

    if (sprite.getPosition().y > stopFallingPosition)
        stop_falling(stopFallingPosition);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        movement.y -= speedRun;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        movement.y += speedRun;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        //sprite.setOrigin({sprite.getLocalBounds().getCenter().x, 0});
        sprite.setScale({1, 1});
        movement.x -= speedRun;
        run();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        //sprite.setOrigin({sprite.getLocalBounds().getCenter().x, 0});
        sprite.setScale({-1, 1});
        movement.x += speedRun;
        run();
    }


    float length = sqrt(movement.x * movement.x + movement.y * movement.y);
    if (length != 0)
        movement /= length;

    if (jumping)
    {
        speed.y += gravity * deltaTime.asSeconds();
        sprite.move(speed * deltaTime.asSeconds());
    }
    else
    {
        sprite.move(movement * speedRun * deltaTime.asSeconds());
    }

}

bool Hedgehog::wasClicked() const
{
    return isPressed;
}
