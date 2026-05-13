#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include <random>
#include "Projectile.h"

std::vector<sf::Texture> Projectile::textures;

void Projectile::ensureTexturesLoaded()
{
    if (!textures.empty())
    {
        return;
    }

    const std::array<const char*, 3> texturePaths{
        "assets/textures/bullet-mushroom-red.png",
        "assets/textures/bullet-mushroom-green.png",
        "assets/textures/bullet-mushroom-blue.png"};

    textures.reserve(texturePaths.size());

    for (const char* path : texturePaths)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(path))
        {
            std::cout << "Failed to load bullet texture: " << path << '\n';
            continue;
        }

        texture.setSmooth(false);
        textures.push_back(std::move(texture));
    }
}

Projectile::Projectile() = default;

void Projectile::activate(const sf::Vector2f position, const float directionX)
{
    sprite.reset();
    active = true;
    velocity = {directionX * 640.f, 0.f};

    ensureTexturesLoaded();

    if (textures.empty())
    {
        active = false;
        return;
    }

    static std::mt19937 generator{std::random_device{}()};
    std::uniform_int_distribution<std::size_t> distribution(0U, textures.size() - 1U);
    const std::size_t textureIndex = distribution(generator);

    sprite.emplace(textures[textureIndex]);
    sprite->setPosition(position);
    sprite->setTextureRect(
        sf::IntRect({0, 0},
                    {static_cast<int>(textures[textureIndex].getSize().x),
                     static_cast<int>(textures[textureIndex].getSize().y)}));
    sprite->setOrigin({
        sprite->getLocalBounds().size.x * 0.5f,
        sprite->getLocalBounds().size.y * 0.5f});
    sprite->setScale({directionX < 0.f ? -0.25f : 0.25f, 0.25f});
}

void Projectile::update(float dt)
{
    if (!active)
    {
        return;
    }

    if (!sprite.has_value())
    {
        active = false;
        return;
    }

    sprite->move(velocity * dt);

    if (sprite->getPosition().x < -100 || sprite->getPosition().x > 2500 ||
        sprite->getPosition().y < -100 || sprite->getPosition().y > 1500)
    {
        deactivate();
    }
}

void Projectile::draw(sf::RenderTarget& target) const
{
    if (sprite.has_value())
    {
        target.draw(*sprite);
    }
}

bool Projectile::isActive() const
{
    return active;
}

sf::FloatRect Projectile::getBounds() const
{
    if (!sprite.has_value())
    {
        return {};
    }

    return sprite->getGlobalBounds();
}

void Projectile::deactivate()
{
    active = false;
    sprite.reset();
}

