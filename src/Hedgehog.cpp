#include "Hedgehog.h"

#include "Projectile.h"

#include <algorithm>
#include <cmath>
#include <iostream>

Hedgehog::Hedgehog(const std::string& texturePath, const sf::Vector2f size)
    : m_sprite(m_texture),
      m_bounds({0.f, 0.f}, {44.f, 56.f}),
      m_size(size),
      m_state(&normalState())
{
    if (!m_texture.loadFromFile(texturePath))
    {
        std::cerr << "Failed to load hedgehog texture: " << texturePath << '\n';
    }

    m_texture.setSmooth(false);

    const sf::Vector2u textureSize = m_texture.getSize();
    if (textureSize.x > 0U && textureSize.y > 0U)
    {
        m_framePixelSize.x = textureSize.x;
        m_framePixelSize.y = std::max(1U, textureSize.y / 2U);
        m_sprite.setTextureRect(
            sf::IntRect({0, 0},
                        {static_cast<int>(m_framePixelSize.x), static_cast<int>(m_framePixelSize.y)}));
        m_sprite.setOrigin({
            static_cast<float>(m_framePixelSize.x) * 0.5f,
            static_cast<float>(m_framePixelSize.y) * 0.5f});
    }

    if (m_state != nullptr)
    {
        m_state->enter(*this);
    }

    refreshSprite();
}

Hedgehog::~Hedgehog() = default;

void Hedgehog::handleEvent(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Space)
        {
            m_jumpRequested = true;
        }
    }
}

void Hedgehog::update(const float deltaTimeSeconds,
                      const std::vector<sf::FloatRect>& collisionRects,
                      const std::vector<sf::FloatRect>& ladderRects)
{
    m_invulnerabilitySeconds = std::max(0.f, m_invulnerabilitySeconds - deltaTimeSeconds);
    refreshGrounded(collisionRects);

    m_running = false;
    m_movement = {0.f, 0.f};

    const HedgehogInput input = sampleInput();
    if (m_state != nullptr)
    {
        m_state->update(*this, input, deltaTimeSeconds, collisionRects, ladderRects);
    }

    updateAnimation(deltaTimeSeconds, input.moveAxis);
    refreshSprite();
    m_jumpRequested = false;
}

void Hedgehog::run()
{
    m_running = true;
}

void Hedgehog::jump()
{
    m_speed.y = -m_jumpForce;
    m_jumping = true;
    m_onGround = false;
}

void Hedgehog::shoot(std::vector<Projectile>& projectiles)
{
    if (m_shootCooldown.getElapsedTime().asSeconds() < m_shootInterval)
    {
        return;
    }

    projectiles.emplace_back(
        sf::Vector2f{
            m_bounds.getCenter().x + (m_facingLeft ? -1.f : 1.f) * (m_bounds.size.x * 0.5f + 10.f),
            m_bounds.position.y + 22.f},
        m_facingLeft ? -1.f : 1.f);

    m_shootCooldown.restart();
}

void Hedgehog::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}

void Hedgehog::setPosition(const sf::Vector2f position)
{
    m_bounds.position = position;
    refreshSprite();
}

void Hedgehog::set_position(const sf::Vector2f position)
{
    setPosition(position);
}

sf::Vector2f Hedgehog::getPosition() const
{
    return m_bounds.position;
}

sf::Vector2f Hedgehog::getCenter() const
{
    return m_bounds.getCenter();
}

const sf::FloatRect& Hedgehog::getBounds() const
{
    return m_bounds;
}

bool Hedgehog::canTakeDamage() const
{
    return m_invulnerabilitySeconds <= 0.f;
}

bool Hedgehog::takeDamage(const float sourceX)
{
    if (!canTakeDamage())
    {
        return false;
    }

    m_invulnerabilitySeconds = 1.f;
    m_speed.x = m_bounds.getCenter().x < sourceX ? -220.f : 220.f;
    m_speed.y = -220.f;
    m_onGround = false;
    return true;
}

void Hedgehog::respawn(const sf::Vector2f position)
{
    m_speed = {0.f, 0.f};
    m_jumpRequested = false;
    m_onGround = false;
    m_onLadder = false;
    m_invulnerabilitySeconds = 0.f;
    setStandingHitbox();
    setPosition(position);
    changeState(normalState());
}

void Hedgehog::changeState(HedgehogState& nextState)
{
    m_state = &nextState;
    m_state->enter(*this);
}

HedgehogInput Hedgehog::sampleInput() const
{
    HedgehogInput input;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
    {
        input.moveAxis -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
    {
        input.moveAxis += 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
    {
        input.climbAxis -= 1.f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
    {
        input.climbAxis += 1.f;
    }

    input.jumpPressed = m_jumpRequested;
    input.rollHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
    return input;
}

void Hedgehog::refreshGrounded(const std::vector<sf::FloatRect>& collisionRects)
{
    const sf::FloatRect groundProbe({m_bounds.position.x, m_bounds.position.y + 1.f}, m_bounds.size);
    if (!m_onLadder)
    {
        m_onGround = intersectsAny(groundProbe, collisionRects);
        m_jumping = !m_onGround;
    }
}

void Hedgehog::applyGravity(const float deltaTimeSeconds)
{
    m_speed.y = std::min(m_speed.y + m_gravity * deltaTimeSeconds, m_maxFallSpeed);
}

void Hedgehog::moveAndResolve(const float deltaTimeSeconds, const std::vector<sf::FloatRect>& collisionRects)
{
    m_bounds.position.x += m_speed.x * deltaTimeSeconds;
    resolveHorizontalCollisions(collisionRects);

    m_bounds.position.y += m_speed.y * deltaTimeSeconds;
    m_onGround = false;
    resolveVerticalCollisions(collisionRects);
}

void Hedgehog::setStandingHitbox()
{
    setHitboxKeepingFeet(m_standingHitbox);
}

void Hedgehog::setRollingHitbox()
{
    setHitboxKeepingFeet(m_rollingHitbox);
}

bool Hedgehog::canStandUp(const std::vector<sf::FloatRect>& collisionRects) const
{
    sf::FloatRect standingBounds = m_bounds;
    const float feetY = m_bounds.position.y + m_bounds.size.y;
    standingBounds.size = m_standingHitbox;
    standingBounds.position.y = feetY - standingBounds.size.y;
    return !intersectsAny(standingBounds, collisionRects);
}

void Hedgehog::setHitboxKeepingFeet(const sf::Vector2f newSize)
{
    const float feetY = m_bounds.position.y + m_bounds.size.y;
    m_bounds.size = newSize;
    m_bounds.position.y = feetY - newSize.y;
}

void Hedgehog::updateFacing(const float moveAxis)
{
    if (moveAxis < 0.f)
    {
        m_facingLeft = true;
    }
    else if (moveAxis > 0.f)
    {
        m_facingLeft = false;
    }
}

bool Hedgehog::intersectsAny(const sf::FloatRect& bounds,
                             const std::vector<sf::FloatRect>& collisionRects) const
{
    return std::any_of(
        collisionRects.begin(),
        collisionRects.end(),
        [&bounds](const sf::FloatRect& collider)
        {
            return bounds.findIntersection(collider).has_value();
        });
}

void Hedgehog::resolveHorizontalCollisions(const std::vector<sf::FloatRect>& collisionRects)
{
    for (const sf::FloatRect& collider : collisionRects)
    {
        if (!m_bounds.findIntersection(collider).has_value())
        {
            continue;
        }

        if (m_speed.x > 0.f)
        {
            m_bounds.position.x = collider.position.x - m_bounds.size.x;
        }
        else if (m_speed.x < 0.f)
        {
            m_bounds.position.x = collider.position.x + collider.size.x;
        }

        m_speed.x = 0.f;
    }
}

void Hedgehog::resolveVerticalCollisions(const std::vector<sf::FloatRect>& collisionRects)
{
    for (const sf::FloatRect& collider : collisionRects)
    {
        if (!m_bounds.findIntersection(collider).has_value())
        {
            continue;
        }

        if (m_speed.y > 0.f)
        {
            m_bounds.position.y = collider.position.y - m_bounds.size.y;
            m_onGround = true;
            m_jumping = false;
        }
        else if (m_speed.y < 0.f)
        {
            m_bounds.position.y = collider.position.y + collider.size.y;
        }

        m_speed.y = 0.f;
    }
}

const sf::FloatRect* Hedgehog::findIntersectingLadder(const std::vector<sf::FloatRect>& ladderRects) const
{
    for (const sf::FloatRect& ladderRect : ladderRects)
    {
        if (m_bounds.findIntersection(ladderRect).has_value())
        {
            return &ladderRect;
        }
    }

    return nullptr;
}

void Hedgehog::snapToLadder(const sf::FloatRect& ladderRect)
{
    m_bounds.position.x = ladderRect.position.x + (ladderRect.size.x - m_bounds.size.x) * 0.5f;
}

void Hedgehog::refreshSprite()
{
    if (m_framePixelSize.x > 0U && m_framePixelSize.y > 0U)
    {
        const float scaleX = m_size.x / static_cast<float>(m_framePixelSize.x);
        const bool isRolling = m_state != nullptr && std::string(m_state->name()) == "Rolling";
        const float scaleY = (m_size.y / static_cast<float>(m_framePixelSize.y)) * (isRolling ? 0.78f : 1.f);
        m_sprite.setScale({m_facingLeft ? -scaleX : scaleX, scaleY});
    }

    const bool blink = m_invulnerabilitySeconds > 0.f &&
                       std::fmod(m_invulnerabilitySeconds * 12.f, 2.f) > 1.f;
    m_sprite.setColor(blink ? sf::Color(255, 255, 255, 120) : sf::Color::White);

    m_sprite.setPosition({
        m_bounds.getCenter().x,
        m_bounds.getCenter().y - (m_size.y - m_bounds.size.y) * 0.5f});
}

void Hedgehog::updateAnimation(const float deltaTimeSeconds, const float moveAxis)
{
    if (m_framePixelSize.x == 0U || m_framePixelSize.y == 0U)
    {
        return;
    }

    const bool isRolling = m_state != nullptr && std::string(m_state->name()) == "Rolling";
    const bool animate = (std::abs(moveAxis) > 0.01f || isRolling) && m_onGround;
    if (!animate)
    {
        m_animationTimer = 0.f;
        m_animationFrame = 0;
    }
    else
    {
        m_animationTimer += deltaTimeSeconds;
        const float frameDuration = isRolling ? 0.12f : 0.18f;
        if (m_animationTimer >= frameDuration)
        {
            m_animationTimer = 0.f;
            m_animationFrame = (m_animationFrame + 1) % 2;
        }
    }

    const int frameY = m_animationFrame * static_cast<int>(m_framePixelSize.y);
    m_sprite.setTextureRect(
        sf::IntRect({0, frameY}, {static_cast<int>(m_framePixelSize.x), static_cast<int>(m_framePixelSize.y)}));
}
