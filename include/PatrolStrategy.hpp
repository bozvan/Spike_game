#pragma once

#include <SFML/System/Vector2.hpp>

class IPatrolStrategy
{
public:
    virtual ~IPatrolStrategy() = default;
    virtual void update(sf::Vector2f& position, float& direction, float deltaTimeSeconds) = 0;
};

class HorizontalPatrolStrategy final : public IPatrolStrategy
{
public:
    // Strategy pattern: Bat can swap patrol logic without changing enemy code.
    HorizontalPatrolStrategy(float minX, float maxX, float speed);
    void update(sf::Vector2f& position, float& direction, float deltaTimeSeconds) override;

private:
    float m_minX{};
    float m_maxX{};
    float m_speed{};
};
