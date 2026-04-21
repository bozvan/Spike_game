#ifndef HEDGEHOG_H
#define HEDGEHOG_H

#include <SFML/Graphics.hpp>

#include "HedgehogState.hpp"

#include <string>
#include <vector>

class Projectile;

class Hedgehog
{
public:
    Hedgehog(const std::string& texturePath, sf::Vector2f size);
    ~Hedgehog();

    void handleEvent(const sf::Event& event);
    void update(float deltaTimeSeconds,
                const std::vector<sf::FloatRect>& collisionRects,
                const std::vector<sf::FloatRect>& ladderRects);
    void run();
    void jump();
    void shoot(std::vector<Projectile>& projectiles);
    void draw(sf::RenderTarget& target) const;

    void setPosition(sf::Vector2f position);
    void set_position(sf::Vector2f position);

    [[nodiscard]] sf::Vector2f getPosition() const;
    [[nodiscard]] sf::Vector2f getCenter() const;
    [[nodiscard]] const sf::FloatRect& getBounds() const;
    [[nodiscard]] bool canTakeDamage() const;
    bool takeDamage(float sourceX);
    void respawn(sf::Vector2f position);

private:
    // State pattern: Hedgehog delegates mode-specific behavior
    // to explicit Normal / Climbing / Rolling state classes.
    friend class HedgehogState;
    friend class HedgehogNormalState;
    friend class HedgehogClimbingState;
    friend class HedgehogRollingState;

    void changeState(HedgehogState& nextState);
    [[nodiscard]] HedgehogInput sampleInput() const;
    void refreshGrounded(const std::vector<sf::FloatRect>& collisionRects);
    void applyGravity(float deltaTimeSeconds);
    void moveAndResolve(float deltaTimeSeconds, const std::vector<sf::FloatRect>& collisionRects);
    void setStandingHitbox();
    void setRollingHitbox();
    [[nodiscard]] bool canStandUp(const std::vector<sf::FloatRect>& collisionRects) const;
    void setHitboxKeepingFeet(sf::Vector2f newSize);
    void updateFacing(float moveAxis);
    [[nodiscard]] bool intersectsAny(const sf::FloatRect& bounds,
                                     const std::vector<sf::FloatRect>& collisionRects) const;
    void resolveHorizontalCollisions(const std::vector<sf::FloatRect>& collisionRects);
    void resolveVerticalCollisions(const std::vector<sf::FloatRect>& collisionRects);
    [[nodiscard]] const sf::FloatRect* findIntersectingLadder(const std::vector<sf::FloatRect>& ladderRects) const;
    void snapToLadder(const sf::FloatRect& ladderRect);
    void refreshSprite();
    void updateAnimation(float deltaTimeSeconds, float moveAxis);

    sf::Texture m_texture;
    sf::Sprite m_sprite;
    sf::FloatRect m_bounds{{0.f, 0.f}, {44.f, 56.f}};
    sf::Vector2f m_size{64.f, 64.f};
    sf::Vector2f m_standingHitbox{44.f, 56.f};
    sf::Vector2f m_rollingHitbox{52.f, 38.f};
    sf::Vector2u m_framePixelSize{};
    HedgehogState* m_state{};

    bool m_running{};
    bool m_jumping{};
    bool m_onGround{};
    bool m_onLadder{};
    bool m_jumpRequested{};
    bool m_facingLeft{};

    float m_jumpForce{430.f};
    float m_gravity{1650.f};
    sf::Vector2f m_speed{};
    sf::Vector2f m_movement{};
    float m_speedRun{210.f};
    float m_ladderSpeed{170.f};
    float m_rollSpeed{260.f};
    float m_maxFallSpeed{1050.f};
    float m_animationTimer{};
    int m_animationFrame{};
    sf::Clock m_shootCooldown;
    float m_shootInterval{0.35f};
    float m_invulnerabilitySeconds{0.f};
};

#endif
