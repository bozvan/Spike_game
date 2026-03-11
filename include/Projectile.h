#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <SFML/Graphics.hpp>

class Projectile {
private:
    static sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f speed;
    bool active;

public:
    Projectile(sf::Vector2f position, float directionX);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool isActive() const;
};

#endif // PROJECTILE_H
