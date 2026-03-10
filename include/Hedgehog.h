#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SFML/Graphics.hpp>

enum Direction
{
    LEFT,
    RIGHT,
    DEFAULT
};

class Hedgehog {
private:
    sf::RectangleShape shape;
    sf::Vector2f movement;
    bool isPressed;
    bool jumping;

    float jump_force;
    float gravity;
    sf::Vector2f speed;
    float speedRun;
    float stopFallingPosition;

public:
    Hedgehog(sf::Vector2f position, sf::Vector2f size, sf::Color color);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void stop_falling(float y);
    void set_position(const sf::Vector2f& pos);
    void draw(sf::RenderWindow& window);
    void jump(Direction direction);
    void update(sf::Time &deltaTime);
    bool wasClicked() const;
};

#endif
