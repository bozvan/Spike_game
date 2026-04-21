#ifndef PROJECTILE_H
#define PROJECTILE_H
#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>

class Projectile
{
private:
    static std::vector<sf::Texture> textures;
    std::optional<sf::Sprite> sprite;
    sf::Vector2f velocity;
    bool active;

    static void ensureTexturesLoaded();

public:
    Projectile(sf::Vector2f position, float directionX);
    void update(float dt);
    void draw(sf::RenderTarget& target) const;
    [[nodiscard]] bool isActive() const;
    [[nodiscard]] sf::FloatRect getBounds() const;
    void deactivate();
};

#endif // PROJECTILE_H
