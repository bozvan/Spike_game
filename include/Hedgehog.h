#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SFML/Graphics.hpp>
#include "Direction.h"
#include "Projectile.h"

class Hedgehog {
private:
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

    sf::Clock shootCooldown;
    const float shootInterval = 0.3f;

public:
    Hedgehog(sf::Vector2f position, std::string &texturePath);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void stop_falling(float y);
    void set_position(const sf::Vector2f& pos);
    void draw(sf::RenderWindow& window);
    void jump(Direction direction);
    void run();
    void shoot(std::vector<Projectile> &projectiles);
    void update(sf::Time &deltaTime, std::vector<Projectile> &projectiles);
    bool wasClicked() const;

    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;
};

#endif // HEDGEHOG_H
