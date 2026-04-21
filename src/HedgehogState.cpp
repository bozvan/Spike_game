#include "HedgehogState.hpp"

#include "Hedgehog.h"

#include <cmath>
const char* HedgehogNormalState::name() const
{
    return "Normal";
}

void HedgehogNormalState::enter(Hedgehog& player)
{
    player.m_onLadder = false;
    player.setStandingHitbox();
}

void HedgehogNormalState::update(Hedgehog& player,
                                 const HedgehogInput& input,
                                 const float deltaTimeSeconds,
                                 const std::vector<sf::FloatRect>& collisionRects,
                                 const std::vector<sf::FloatRect>& ladderRects)
{
    if (player.findIntersectingLadder(ladderRects) != nullptr && std::abs(input.climbAxis) > 0.01f)
    {
        player.changeState(climbingState());
        return;
    }

    if (input.rollHeld && player.m_onGround)
    {
        player.changeState(rollingState());
        return;
    }

    player.updateFacing(input.moveAxis);
    player.m_movement.x = input.moveAxis;
    player.m_speed.x = input.moveAxis * player.m_speedRun;

    if (std::abs(input.moveAxis) > 0.01f)
    {
        player.run();
    }

    if (input.jumpPressed && player.m_onGround)
    {
        player.jump();
    }

    if (!player.m_onGround)
    {
        player.applyGravity(deltaTimeSeconds);
    }
    else if (player.m_speed.y > 0.f)
    {
        player.m_speed.y = 0.f;
    }

    player.moveAndResolve(deltaTimeSeconds, collisionRects);
    if (!player.m_onLadder)
    {
        player.refreshGrounded(collisionRects);
    }
}

const char* HedgehogClimbingState::name() const
{
    return "Climbing";
}

void HedgehogClimbingState::enter(Hedgehog& player)
{
    player.m_onLadder = true;
    player.m_speed = {0.f, 0.f};
    player.setStandingHitbox();
}

void HedgehogClimbingState::update(Hedgehog& player,
                                   const HedgehogInput& input,
                                   const float deltaTimeSeconds,
                                   const std::vector<sf::FloatRect>& collisionRects,
                                   const std::vector<sf::FloatRect>& ladderRects)
{
    const sf::FloatRect* ladderRect = player.findIntersectingLadder(ladderRects);
    if (ladderRect == nullptr)
    {
        player.m_onLadder = false;
        player.changeState(normalState());
        return;
    }

    player.snapToLadder(*ladderRect);
    player.updateFacing(input.moveAxis);

    if (input.jumpPressed)
    {
        player.m_onLadder = false;
        player.changeState(normalState());
        player.jump();
        return;
    }

    if (std::abs(input.moveAxis) > 0.01f && std::abs(input.climbAxis) < 0.01f)
    {
        player.m_onLadder = false;
        player.changeState(normalState());
        return;
    }

    player.m_speed.x = 0.f;
    player.m_speed.y = input.climbAxis * player.m_ladderSpeed;
    player.moveAndResolve(deltaTimeSeconds, collisionRects);
    player.m_onLadder = true;

    if (const sf::FloatRect* ladderAfterMove = player.findIntersectingLadder(ladderRects))
    {
        player.snapToLadder(*ladderAfterMove);
    }
}

const char* HedgehogRollingState::name() const
{
    return "Rolling";
}

void HedgehogRollingState::enter(Hedgehog& player)
{
    player.m_onLadder = false;
    player.setRollingHitbox();
}

void HedgehogRollingState::update(Hedgehog& player,
                                  const HedgehogInput& input,
                                  const float deltaTimeSeconds,
                                  const std::vector<sf::FloatRect>& collisionRects,
                                  const std::vector<sf::FloatRect>& ladderRects)
{
    if (!input.rollHeld && player.canStandUp(collisionRects))
    {
        player.changeState(normalState());
        return;
    }

    if (!input.rollHeld &&
        player.findIntersectingLadder(ladderRects) != nullptr &&
        std::abs(input.climbAxis) > 0.01f)
    {
        player.changeState(climbingState());
        return;
    }

    player.updateFacing(input.moveAxis);
    player.m_speed.x = input.moveAxis * player.m_rollSpeed;
    player.m_movement.x = input.moveAxis;

    if (std::abs(input.moveAxis) > 0.01f)
    {
        player.run();
    }

    if (!player.m_onGround)
    {
        player.applyGravity(deltaTimeSeconds);
    }
    else if (player.m_speed.y > 0.f)
    {
        player.m_speed.y = 0.f;
    }

    player.moveAndResolve(deltaTimeSeconds, collisionRects);
    if (!player.m_onLadder)
    {
        player.refreshGrounded(collisionRects);
    }
}

HedgehogState& normalState()
{
    static HedgehogNormalState state;
    return state;
}

HedgehogState& climbingState()
{
    static HedgehogClimbingState state;
    return state;
}

HedgehogState& rollingState()
{
    static HedgehogRollingState state;
    return state;
}
