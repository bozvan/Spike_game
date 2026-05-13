#pragma once

#include "Projectile.h"

#include <cstddef>
#include <vector>

class ProjectilePool
{
public:
    explicit ProjectilePool(std::size_t capacity);

    [[nodiscard]] bool trySpawn(sf::Vector2f position, float directionX);
    void deactivateAll();

    using iterator = std::vector<Projectile>::iterator;
    using const_iterator = std::vector<Projectile>::const_iterator;

    [[nodiscard]] iterator begin();
    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;

private:
    std::vector<Projectile> m_slots;
};
