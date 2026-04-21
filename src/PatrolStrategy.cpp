#include "PatrolStrategy.hpp"

#include <algorithm>
#include <cmath>

HorizontalPatrolStrategy::HorizontalPatrolStrategy(const float minX, const float maxX, const float speed)
    : m_minX(std::min(minX, maxX)),
      m_maxX(std::max(minX, maxX)),
      m_speed(std::abs(speed))
{
}

void HorizontalPatrolStrategy::update(sf::Vector2f& position,
                                      float& direction,
                                      const float deltaTimeSeconds)
{
    position.x += direction * m_speed * deltaTimeSeconds;

    if (position.x <= m_minX)
    {
        position.x = m_minX;
        direction = 1.f;
    }
    else if (position.x >= m_maxX)
    {
        position.x = m_maxX;
        direction = -1.f;
    }
}
