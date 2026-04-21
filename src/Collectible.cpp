#include "Collectible.hpp"

#include <iostream>
#include <utility>

KeyCollectible::KeyCollectible(std::string objectId, const sf::FloatRect bounds)
    : m_objectId(std::move(objectId)),
      m_bounds(bounds)
{
}

bool KeyCollectible::tryCollect(const sf::FloatRect& playerBounds, ProgressModel& progress)
{
    if (!m_active || !playerBounds.findIntersection(m_bounds).has_value())
    {
        return false;
    }

    if (!progress.collectKey(m_objectId))
    {
        return false;
    }

    m_active = false;
    std::cout << "Picked up key (" << progress.getCollectedKeysInCurrentLevel()
              << '/' << progress.getRequiredKeysInCurrentLevel() << ")\n";
    return true;
}

bool KeyCollectible::isActive() const
{
    return m_active;
}

void KeyCollectible::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_active)
    {
        return;
    }

    sf::CircleShape ring(7.f);
    ring.setFillColor(sf::Color::Transparent);
    ring.setOutlineThickness(4.f);
    ring.setOutlineColor(sf::Color(245, 210, 70));
    ring.setPosition({m_bounds.position.x + 4.f, m_bounds.position.y + 6.f});
    target.draw(ring, states);

    sf::RectangleShape stem({14.f, 4.f});
    stem.setFillColor(sf::Color(245, 210, 70));
    stem.setPosition({m_bounds.position.x + 18.f, m_bounds.position.y + 14.f});
    target.draw(stem, states);

    sf::RectangleShape tooth({4.f, 8.f});
    tooth.setFillColor(sf::Color(245, 210, 70));
    tooth.setPosition({m_bounds.position.x + 26.f, m_bounds.position.y + 14.f});
    target.draw(tooth, states);

    tooth.setPosition({m_bounds.position.x + 30.f, m_bounds.position.y + 10.f});
    target.draw(tooth, states);
}

PartCollectible::PartCollectible(std::string objectId, const sf::FloatRect bounds, const PartColor color)
    : m_objectId(std::move(objectId)),
      m_bounds(bounds),
      m_color(color)
{
}

bool PartCollectible::tryCollect(const sf::FloatRect& playerBounds, ProgressModel& progress)
{
    if (!m_active || !playerBounds.findIntersection(m_bounds).has_value())
    {
        return false;
    }

    if (!progress.collectPart(m_objectId, m_color))
    {
        return false;
    }

    m_active = false;
    std::cout << "Collected part: "
              << (m_color == PartColor::Red ? "red" : m_color == PartColor::Blue ? "blue" : "green")
              << '\n';
    return true;
}

bool PartCollectible::isActive() const
{
    return m_active;
}

void PartCollectible::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_active)
    {
        return;
    }

    sf::ConvexShape gem;
    gem.setPointCount(4);
    gem.setPoint(0, {m_bounds.getCenter().x, m_bounds.position.y});
    gem.setPoint(1, {m_bounds.position.x + m_bounds.size.x, m_bounds.getCenter().y});
    gem.setPoint(2, {m_bounds.getCenter().x, m_bounds.position.y + m_bounds.size.y});
    gem.setPoint(3, {m_bounds.position.x, m_bounds.getCenter().y});
    gem.setFillColor(getColor());
    gem.setOutlineThickness(2.f);
    gem.setOutlineColor(sf::Color(255, 255, 255, 160));
    target.draw(gem, states);
}

sf::Color PartCollectible::getColor() const
{
    switch (m_color)
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
