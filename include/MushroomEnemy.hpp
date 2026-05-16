#pragma once

#include "Enemy.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class Hedgehog;
class ProgressModel;
class Projectile;
class IPatrolStrategy;

class MushroomEnemy final : public Enemy
{
public:
    MushroomEnemy(std::string objectId,
        sf::Vector2f center,
        sf::Vector2f size,
        std::unique_ptr<IPatrolStrategy> patrolStrategy,
        float speed = 40.0f);

    void update(float deltaTimeSeconds) override;
    bool tryHitByProjectile(Projectile& projectile) override;
    bool tryAttackPlayer(Hedgehog& player, ProgressModel& progress) override;
    [[nodiscard]] bool isActive() const override;

    void setSpeed(float speed) override { m_speed = speed; }
    [[nodiscard]] float getSpeed() const override { return m_speed; }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    [[nodiscard]] sf::FloatRect getBounds() const;

    std::string m_objectId;
    sf::Vector2f m_center;
    sf::Vector2f m_size;
    std::unique_ptr<IPatrolStrategy> m_patrolStrategy;
    float m_direction{ 1.f };
    float m_speed{ 40.0f };
    bool m_active{ true };
    int m_health{ 2 };

    float m_hurtTimer{ 0.0f };
    bool m_isFacingRight{ true };
};