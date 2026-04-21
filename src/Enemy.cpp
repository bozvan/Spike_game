#include "Enemy.hpp"

#include "Hedgehog.h"
#include "PatrolStrategy.hpp"
#include "ProgressModel.hpp"
#include "Projectile.h"

#include <cmath>
#include <iostream>
#include <utility>

namespace
{
constexpr float pi = 3.14159265358979323846f;
}

BatEnemy::BatEnemy(std::string objectId,
                   const sf::Vector2f center,
                   const sf::Vector2f size,
                   std::unique_ptr<IPatrolStrategy> patrolStrategy)
    : m_objectId(std::move(objectId)),
      m_center(center),
      m_size(size),
      m_patrolStrategy(std::move(patrolStrategy))
{
}

void BatEnemy::update(const float deltaTimeSeconds)
{
    if (!m_active)
    {
        return;
    }

    m_bobTimer += deltaTimeSeconds;
    if (m_patrolStrategy != nullptr)
    {
        m_patrolStrategy->update(m_center, m_direction, deltaTimeSeconds);
    }
}

bool BatEnemy::tryHitByProjectile(Projectile& projectile)
{
    if (!m_active || !projectile.isActive())
    {
        return false;
    }

    if (!getBounds().findIntersection(projectile.getBounds()).has_value())
    {
        return false;
    }

    m_active = false;
    projectile.deactivate();
    std::cout << "Bat defeated: " << m_objectId << '\n';
    return true;
}

bool BatEnemy::tryAttackPlayer(Hedgehog& player, ProgressModel& progress)
{
    if (!m_active)
    {
        return false;
    }

    if (!getBounds().findIntersection(player.getBounds()).has_value())
    {
        return false;
    }

    if (!player.takeDamage(getBounds().getCenter().x))
    {
        return false;
    }

    progress.loseLife();
    std::cout << "Bat hit the player. Lives left: " << progress.getLives() << '\n';
    return true;
}

bool BatEnemy::isActive() const
{
    return m_active;
}

void BatEnemy::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_active)
    {
        return;
    }

    const sf::FloatRect bounds = getBounds();
    const float wingOffset = std::sin(m_bobTimer * 12.f) * 8.f;

    sf::ConvexShape leftWing;
    leftWing.setPointCount(3);
    leftWing.setPoint(0, {bounds.position.x + bounds.size.x * 0.45f, bounds.position.y + bounds.size.y * 0.4f});
    leftWing.setPoint(1, {bounds.position.x - 8.f, bounds.position.y + bounds.size.y * 0.15f + wingOffset});
    leftWing.setPoint(2, {bounds.position.x + bounds.size.x * 0.2f, bounds.position.y + bounds.size.y * 0.85f});
    leftWing.setFillColor(sf::Color(65, 57, 87));
    target.draw(leftWing, states);

    sf::ConvexShape rightWing;
    rightWing.setPointCount(3);
    rightWing.setPoint(0, {bounds.position.x + bounds.size.x * 0.55f, bounds.position.y + bounds.size.y * 0.4f});
    rightWing.setPoint(1, {bounds.position.x + bounds.size.x + 8.f, bounds.position.y + bounds.size.y * 0.15f - wingOffset});
    rightWing.setPoint(2, {bounds.position.x + bounds.size.x * 0.8f, bounds.position.y + bounds.size.y * 0.85f});
    rightWing.setFillColor(sf::Color(65, 57, 87));
    target.draw(rightWing, states);

    sf::CircleShape body(bounds.size.y * 0.4f);
    body.setScale({bounds.size.x / (bounds.size.y * 0.8f), 1.f});
    body.setOrigin({body.getRadius(), body.getRadius()});
    body.setPosition(bounds.getCenter());
    body.setFillColor(sf::Color(99, 81, 128));
    target.draw(body, states);

    sf::CircleShape eye(2.5f);
    eye.setFillColor(sf::Color(255, 240, 180));
    eye.setPosition({bounds.position.x + bounds.size.x * 0.32f, bounds.position.y + bounds.size.y * 0.3f});
    target.draw(eye, states);
    eye.setPosition({bounds.position.x + bounds.size.x * 0.56f, bounds.position.y + bounds.size.y * 0.3f});
    target.draw(eye, states);
}

sf::FloatRect BatEnemy::getBounds() const
{
    const float bobOffset = std::sin(m_bobTimer * 4.f) * 4.f;
    return {
        {m_center.x - m_size.x * 0.5f, m_center.y - m_size.y * 0.5f + bobOffset},
        m_size};
}
