#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Projectile;

enum class Direction
{
    LEFT,
    RIGHT,
    DEFAULT
};

class Hedgehog
{
private:

    sf::Texture texture;
    sf::Sprite sprite;

    sf::Image image;
    std::vector<std::vector<bool>> alphaMask;

    bool isPressed;
    bool running;
    bool jumping;
    bool onGround;

    float jump_force;
    float gravity;

    sf::Vector2f speed;
    sf::Vector2f movement;

    float speedRun;

    int state;

    sf::Clock shootCooldown;
    float shootInterval;

public:

    Hedgehog(sf::Vector2f position, std::string& texturePath);

    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;

    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);

    void update(sf::Time& deltaTime,
                std::vector<Projectile>& projectiles,
                sf::Sprite& otherSprite,
                const std::vector<std::vector<bool>>& otherMask);

    void jump(Direction direction = Direction::DEFAULT);
    void run();
    void shoot(std::vector<Projectile>& projectiles);

    void set_position(const sf::Vector2f& pos);

    bool isCollision(sf::Sprite& otherSprite) const;
    void resolveCollision(sf::Sprite& otherSprite,
                          const std::vector<std::vector<bool>>& otherMask);
    void resolveHorizontalCollision(sf::Sprite& otherSprite,
                                    const std::vector<std::vector<bool>>& otherMask,
                                    float oldX);
    void resolveVerticalCollision(sf::Sprite& otherSprite,
                                  const std::vector<std::vector<bool>>& otherMask,
                                  float oldY);

    bool pixelPerfectCollision(sf::Sprite& otherSprite,
                               const std::vector<std::vector<bool>>& otherMask);

    bool wasClicked() const;

    void createAlphaMask();
};

#endif
