#include <SFML/Graphics.hpp>
#include <iostream>
#include "Projectile.h"

sf::Texture Projectile::texture;

Projectile::Projectile(sf::Vector2f position, float directionX) :
    sprite(texture),
    active(true),
    speed(directionX * 800.f, 0.f)
{
    static bool loaded = false;
    if (!loaded) {
        if (!texture.loadFromFile("assets/textures/bullet-mushroom.png")) {
            std::cout << "Failed to load bullet texture!\n";
        } else {
            std::cout << "Bullet texture loaded successfully.\n";
        }
        loaded = true;
    }

    sprite.setPosition(position);
    sprite.setTextureRect(sf::IntRect({0, 0},
                                      {static_cast<int>(texture.getSize().x), static_cast<int>(texture.getSize().y)}));
    sprite.setOrigin({sprite.getLocalBounds().getCenter().x, 0.f});
}

void Projectile::update(float dt) {
    sprite.move(speed * dt);
    if (sprite.getPosition().x < -100 || sprite.getPosition().x > 2000 ||
        sprite.getPosition().y < -100 || sprite.getPosition().y > 2000) {
        active = false;
    }
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

bool Projectile::isActive() const
{
    return active;
}

