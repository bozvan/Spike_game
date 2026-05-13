#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

class ProjectilePool;

class Projectile
{
    friend class ProjectilePool;

private:
    static std::vector<sf::Texture> textures;
    std::optional<sf::Sprite> sprite;
    sf::Vector2f velocity{};
    bool active{false};

    static void ensureTexturesLoaded();
    void activate(sf::Vector2f position, float directionX);

public:
    Projectile();
    void update(float dt);
    void draw(sf::RenderTarget& target) const;
    [[nodiscard]] bool isActive() const;
    [[nodiscard]] sf::FloatRect getBounds() const;
    void deactivate();
};

#endif
