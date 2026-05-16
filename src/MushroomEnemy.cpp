#include "MushroomEnemy.hpp"
#include "Hedgehog.h"
#include "PatrolStrategy.hpp"
#include "ProgressModel.hpp"
#include "Projectile.h"

#include <cmath>
#include <iostream>
#include <utility>

MushroomEnemy::MushroomEnemy(std::string objectId,
    const sf::Vector2f center,
    const sf::Vector2f size,
    std::unique_ptr<IPatrolStrategy> patrolStrategy,
    float speed)
    : m_objectId(std::move(objectId))
    , m_center(center)
    , m_size(size)
    , m_patrolStrategy(std::move(patrolStrategy))
    , m_speed(speed)
{
}

void MushroomEnemy::update(const float deltaTimeSeconds)
{
    if (!m_active) {
        return;
    }


    if (m_hurtTimer > 0) {
        m_hurtTimer -= deltaTimeSeconds;
    }

    // Движение
    if (m_patrolStrategy != nullptr) {
        m_patrolStrategy->update(m_center, m_direction, deltaTimeSeconds, m_speed);
        m_isFacingRight = m_direction > 0;
    }
}

bool MushroomEnemy::tryHitByProjectile(Projectile& projectile)
{
    if (!m_active || !projectile.isActive()) {
        return false;
    }

    if (!getBounds().findIntersection(projectile.getBounds()).has_value()) {
        return false;
    }

    m_health--;
    m_hurtTimer = 0.2f;
    projectile.deactivate();

    if (m_health <= 0) {
        m_active = false;
        std::cout << "Mushroom defeated!\n";
    }

    return true;
}

bool MushroomEnemy::tryAttackPlayer(Hedgehog& player, ProgressModel& progress)
{
    if (!m_active) {
        return false;
    }

    if (getBounds().findIntersection(player.getBounds()).has_value()) {
        if (player.takeDamage(getBounds().getCenter().x)) {
            progress.loseLife();
            return true;
        }
    }

    return false;
}

bool MushroomEnemy::isActive() const
{
    return m_active;
}

void MushroomEnemy::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_active) {
        return;
    }

    // Мигание при получении урона
    if (m_hurtTimer > 0 && static_cast<int>(m_hurtTimer * 20) % 2 == 0) {
        return;
    }

    const sf::FloatRect bounds = getBounds();
    const float centerX = bounds.getCenter().x;
    const float centerY = bounds.getCenter().y;

    sf::RectangleShape leg({ 12.f, 16.f });
    leg.setOrigin({ 6.f, 8.f });
    leg.setPosition({ centerX, centerY + 8.f });
    leg.setFillColor(sf::Color(200, 200, 200));
    target.draw(leg, states);

    sf::CircleShape hat(14.f);
    hat.setOrigin({ 14.f, 14.f });
    hat.setPosition({ centerX, centerY - 2.f });
    hat.setFillColor(sf::Color(255, 0, 0));  // Красная
    target.draw(hat, states);


    sf::CircleShape leftEye(3.f);
    leftEye.setOrigin({ 3.f, 3.f });
    leftEye.setPosition({ centerX - 6.f, centerY - 4.f });
    leftEye.setFillColor(sf::Color::White);
    target.draw(leftEye, states);

    // Правый глаз
    sf::CircleShape rightEye(3.f);
    rightEye.setOrigin({ 3.f, 3.f });
    rightEye.setPosition({ centerX + 6.f, centerY - 4.f });
    rightEye.setFillColor(sf::Color::White);
    target.draw(rightEye, states);

    float offset = m_isFacingRight ? 1.5f : -1.5f;

    sf::CircleShape leftPupil(1.5f);
    leftPupil.setOrigin({ 1.5f, 1.5f });
    leftPupil.setPosition({ centerX - 5.f + offset, centerY - 3.f });
    leftPupil.setFillColor(sf::Color::Black);
    target.draw(leftPupil, states);

    sf::CircleShape rightPupil(1.5f);
    rightPupil.setOrigin({ 1.5f, 1.5f });
    rightPupil.setPosition({ centerX + 7.f + offset, centerY - 3.f });
    rightPupil.setFillColor(sf::Color::Black);
    target.draw(rightPupil, states);
}

sf::FloatRect MushroomEnemy::getBounds() const
{
    return {
        {m_center.x - m_size.x * 0.4f, m_center.y - m_size.y * 0.3f},
        {m_size.x * 0.8f, m_size.y * 0.7f}
    };
}