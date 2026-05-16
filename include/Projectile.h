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

    std::optional<sf::Sprite> sprite;  // Sprite существует только у активных снар€дов
    sf::Vector2f velocity{ 0, 0 };       // скорость
    bool active{ false };                // активен ли снар€д

    static void ensureTexturesLoaded();// проверка загрузки

    void activate(sf::Vector2f position, float directionX); 

public:
    Projectile(); // пустой снар€д неактивный при начале
    void update(float dt); // каждый кадр дл€ каждого активного снар€да
    void draw(sf::RenderTarget& target) const; // рисует снар€д на экране
    [[nodiscard]] bool isActive() const; // ¬озвращает true, если снар€д сейчас в полЄте
    [[nodiscard]] sf::FloatRect getBounds() const; // »спользуетс€ дл€ обнаружени€ столкновений
    void deactivate(); // если вылетел за экран
};

#endif
