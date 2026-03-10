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
    sf::Texture texture;
    sf::Sprite sprite;
    bool isPressed;
    bool jumping;
    bool running;
    short state;

    float jump_force;
    float gravity;
    sf::Vector2f speed;
    float speedRun;
    float stopFallingPosition;

public:
    Hedgehog(sf::Vector2f position, sf::Color color, std::string &texturePath);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void stop_falling(float y);
    void set_position(const sf::Vector2f& pos);
    void draw(sf::RenderWindow& window);
    void jump(Direction direction);
    void run();
    void update(sf::Time &deltaTime);
    bool wasClicked() const;

    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;
};

#endif
