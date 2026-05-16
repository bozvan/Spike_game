#include "ProjectilePool.hpp"

ProjectilePool::ProjectilePool(const std::size_t capacity)
{
    m_slots.resize(capacity); // выделение памяти под capacity снарядов в векторе.
}

bool ProjectilePool::trySpawn(const sf::Vector2f position, const float directionX)
{
    for (Projectile& slot : m_slots)
    {
        if (!slot.isActive())
        {
            slot.activate(position, directionX);
            return slot.isActive();
        }
    }

    return false;
}

void ProjectilePool::deactivateAll()
{
    for (Projectile& slot : m_slots)
    {
        slot.deactivate();
    }
}

ProjectilePool::iterator ProjectilePool::begin()
{
    return m_slots.begin();
}

ProjectilePool::iterator ProjectilePool::end()
{
    return m_slots.end();
}

ProjectilePool::const_iterator ProjectilePool::begin() const
{
    return m_slots.begin();
}

ProjectilePool::const_iterator ProjectilePool::end() const
{
    return m_slots.end();
}
