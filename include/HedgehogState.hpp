#pragma once

#include <vector>

#include <SFML/Graphics/Rect.hpp>

class Hedgehog;

struct HedgehogInput
{
    float moveAxis{};
    float climbAxis{};
    bool jumpPressed{};
    bool rollHeld{};
};

class HedgehogState
{
public:
    virtual ~HedgehogState() = default;
    virtual const char* name() const = 0;
    virtual void enter(Hedgehog& player) = 0;
    virtual void update(Hedgehog& player,
                        const HedgehogInput& input,
                        float deltaTimeSeconds,
                        const std::vector<sf::FloatRect>& collisionRects,
                        const std::vector<sf::FloatRect>& ladderRects) = 0;
};

class HedgehogNormalState final : public HedgehogState
{
public:
    const char* name() const override;
    void enter(Hedgehog& player) override;
    void update(Hedgehog& player,
                const HedgehogInput& input,
                float deltaTimeSeconds,
                const std::vector<sf::FloatRect>& collisionRects,
                const std::vector<sf::FloatRect>& ladderRects) override;
};

class HedgehogClimbingState final : public HedgehogState
{
public:
    const char* name() const override;
    void enter(Hedgehog& player) override;
    void update(Hedgehog& player,
                const HedgehogInput& input,
                float deltaTimeSeconds,
                const std::vector<sf::FloatRect>& collisionRects,
                const std::vector<sf::FloatRect>& ladderRects) override;
};

class HedgehogRollingState final : public HedgehogState
{
public:
    const char* name() const override;
    void enter(Hedgehog& player) override;
    void update(Hedgehog& player,
                const HedgehogInput& input,
                float deltaTimeSeconds,
                const std::vector<sf::FloatRect>& collisionRects,
                const std::vector<sf::FloatRect>& ladderRects) override;
};

HedgehogState& normalState();
HedgehogState& climbingState();
HedgehogState& rollingState();
