#include "HudOverlay.hpp"

#include <array>
#include <string>

void HudOverlay::onProgressChanged(const ProgressModel& progress)
{
    m_lives = progress.getLives();
    m_keysCollected = progress.getCollectedKeyCount();
    m_keysRequired = progress.getRequiredKeyCount();
    m_hasRedPart = progress.hasPart(PartColor::Red);
    m_hasBluePart = progress.hasPart(PartColor::Blue);
    m_hasGreenPart = progress.hasPart(PartColor::Green);
}

void HudOverlay::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape panel({252.f, 86.f});
    panel.setPosition({16.f, 16.f});
    panel.setFillColor(sf::Color(24, 31, 43, 190));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(130, 163, 202, 180));
    target.draw(panel, states);

    for (int index = 0; index < 3; ++index)
    {
        drawHeart(target, {28.f + index * 28.f, 28.f}, index < m_lives);
    }

    drawPartIndicator(target, {142.f, 30.f}, PartColor::Red, m_hasRedPart);
    drawPartIndicator(target, {172.f, 30.f}, PartColor::Blue, m_hasBluePart);
    drawPartIndicator(target, {202.f, 30.f}, PartColor::Green, m_hasGreenPart);

    drawKeyIndicator(target, {28.f, 58.f}, m_keysCollected > 0U);
    drawCounterText(
        target,
        {50.f, 57.f},
        std::to_string(m_keysCollected) + "/" + std::to_string(m_keysRequired),
        sf::Color(245, 210, 70));
}

void HudOverlay::drawHeart(sf::RenderTarget& target, const sf::Vector2f position, const bool filled) const
{
    const sf::Color fill = filled ? sf::Color(230, 79, 95) : sf::Color(78, 85, 102);

    sf::CircleShape left(7.f);
    left.setFillColor(fill);
    left.setPosition(position);
    target.draw(left);

    sf::CircleShape right(7.f);
    right.setFillColor(fill);
    right.setPosition({position.x + 10.f, position.y});
    target.draw(right);

    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    triangle.setPoint(0, {position.x - 1.f, position.y + 8.f});
    triangle.setPoint(1, {position.x + 18.f, position.y + 8.f});
    triangle.setPoint(2, {position.x + 8.5f, position.y + 22.f});
    triangle.setFillColor(fill);
    target.draw(triangle);
}

void HudOverlay::drawPartIndicator(sf::RenderTarget& target,
                                   const sf::Vector2f position,
                                   const PartColor color,
                                   const bool collected) const
{
    sf::RectangleShape frame({22.f, 22.f});
    frame.setPosition(position);
    frame.setFillColor(sf::Color(36, 45, 60, 220));
    frame.setOutlineThickness(2.f);
    frame.setOutlineColor(sf::Color(140, 153, 174));
    target.draw(frame);

    sf::ConvexShape gem;
    gem.setPointCount(4);
    gem.setPoint(0, {position.x + 11.f, position.y + 2.f});
    gem.setPoint(1, {position.x + 20.f, position.y + 11.f});
    gem.setPoint(2, {position.x + 11.f, position.y + 20.f});
    gem.setPoint(3, {position.x + 2.f, position.y + 11.f});
    const sf::Color baseColor = colorForPart(color);
    gem.setFillColor(collected ? baseColor : sf::Color(baseColor.r / 4, baseColor.g / 4, baseColor.b / 4, 180));
    gem.setOutlineThickness(1.f);
    gem.setOutlineColor(sf::Color(255, 255, 255, collected ? 150 : 70));
    target.draw(gem);
}

void HudOverlay::drawKeyIndicator(sf::RenderTarget& target,
                                  const sf::Vector2f position,
                                  const bool collected) const
{
    const sf::Color color = collected ? sf::Color(245, 210, 70) : sf::Color(90, 90, 90);

    sf::CircleShape ring(4.f);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(2.f);
    ring.setOutlineColor(color);
    ring.setPosition(position);
    target.draw(ring);

    sf::RectangleShape stem({8.f, 2.f});
    stem.setFillColor(color);
    stem.setPosition({position.x + 9.f, position.y + 5.f});
    target.draw(stem);

    sf::RectangleShape tooth({2.f, 4.f});
    tooth.setFillColor(color);
    tooth.setPosition({position.x + 15.f, position.y + 5.f});
    target.draw(tooth);
}

void HudOverlay::drawCounterText(sf::RenderTarget& target,
                                 const sf::Vector2f position,
                                 const std::string& text,
                                 const sf::Color color) const
{
    float cursorX = position.x;
    for (const char symbol : text)
    {
        if (symbol >= '0' && symbol <= '9')
        {
            drawDigitGlyph(target, {cursorX, position.y}, static_cast<unsigned int>(symbol - '0'), color);
            cursorX += 14.f;
            continue;
        }

        if (symbol == '/')
        {
            drawSlashGlyph(target, {cursorX, position.y}, color);
            cursorX += 10.f;
        }
    }
}

void HudOverlay::drawDigitGlyph(sf::RenderTarget& target,
                                const sf::Vector2f position,
                                const unsigned int digit,
                                const sf::Color color) const
{
    static constexpr std::array<std::array<bool, 7>, 10> segments{{
        {{true, true, true, false, true, true, true}},
        {{false, false, true, false, false, true, false}},
        {{true, false, true, true, true, false, true}},
        {{true, false, true, true, false, true, true}},
        {{false, true, true, true, false, true, false}},
        {{true, true, false, true, false, true, true}},
        {{true, true, false, true, true, true, true}},
        {{true, false, true, false, false, true, false}},
        {{true, true, true, true, true, true, true}},
        {{true, true, true, true, false, true, true}},
    }};

    if (digit >= segments.size())
    {
        return;
    }

    const auto drawSegment = [&](const sf::Vector2f segmentPosition, const sf::Vector2f size)
    {
        sf::RectangleShape segment(size);
        segment.setPosition(position + segmentPosition);
        segment.setFillColor(color);
        target.draw(segment);
    };

    if (segments[digit][0]) drawSegment({2.f, 0.f}, {8.f, 2.f});
    if (segments[digit][1]) drawSegment({0.f, 2.f}, {2.f, 7.f});
    if (segments[digit][2]) drawSegment({10.f, 2.f}, {2.f, 7.f});
    if (segments[digit][3]) drawSegment({2.f, 8.f}, {8.f, 2.f});
    if (segments[digit][4]) drawSegment({0.f, 10.f}, {2.f, 7.f});
    if (segments[digit][5]) drawSegment({10.f, 10.f}, {2.f, 7.f});
    if (segments[digit][6]) drawSegment({2.f, 16.f}, {8.f, 2.f});
}

void HudOverlay::drawSlashGlyph(sf::RenderTarget& target, const sf::Vector2f position, const sf::Color color) const
{
    sf::RectangleShape slash({2.f, 18.f});
    slash.setOrigin({1.f, 9.f});
    slash.setPosition({position.x + 4.f, position.y + 9.f});
    slash.setRotation(sf::degrees(24.f));
    slash.setFillColor(color);
    target.draw(slash);
}

sf::Color HudOverlay::colorForPart(const PartColor color)
{
    switch (color)
    {
    case PartColor::Red:
        return sf::Color(220, 71, 71);
    case PartColor::Blue:
        return sf::Color(68, 139, 255);
    case PartColor::Green:
        return sf::Color(76, 194, 104);
    }

    return sf::Color::White;
}
