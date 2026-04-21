#pragma once

#include <SFML/Graphics.hpp>

#include <memory>
#include <string>

class Hedgehog;
class ProgressModel;
class Projectile;
class IPatrolStrategy;

class Enemy : public sf::Drawable
{
public:
    virtual ~Enemy() = default;
    virtual void update(float deltaTimeSeconds) = 0;
    virtual bool tryHitByProjectile(Projectile& projectile) = 0;
    virtual bool tryAttackPlayer(Hedgehog& player, ProgressModel& progress) = 0;
    [[nodiscard]] virtual bool isActive() const = 0;
};

class BatEnemy final : public Enemy
{
public:
    BatEnemy(std::string objectId,
             sf::Vector2f center,
             sf::Vector2f size,
             std::unique_ptr<IPatrolStrategy> patrolStrategy);

    void update(float deltaTimeSeconds) override;
    bool tryHitByProjectile(Projectile& projectile) override;
    bool tryAttackPlayer(Hedgehog& player, ProgressModel& progress) override;
    [[nodiscard]] bool isActive() const override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    [[nodiscard]] sf::FloatRect getBounds() const;

    std::string m_objectId;
    sf::Vector2f m_center;
    sf::Vector2f m_size;
    std::unique_ptr<IPatrolStrategy> m_patrolStrategy;
    float m_direction{1.f};
    float m_bobTimer{};
    bool m_active{true};
};
