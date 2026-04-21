#pragma once

#include "ProgressModel.hpp"

#include <SFML/Graphics.hpp>

#include <string>

class Collectible : public sf::Drawable
{
public:
    virtual ~Collectible() = default;
    virtual bool tryCollect(const sf::FloatRect& playerBounds, ProgressModel& progress) = 0;
    [[nodiscard]] virtual bool isActive() const = 0;
};

class KeyCollectible final : public Collectible
{
public:
    KeyCollectible(std::string objectId, sf::FloatRect bounds);
    bool tryCollect(const sf::FloatRect& playerBounds, ProgressModel& progress) override;
    [[nodiscard]] bool isActive() const override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::string m_objectId;
    sf::FloatRect m_bounds;
    bool m_active{true};
};

class PartCollectible final : public Collectible
{
public:
    PartCollectible(std::string objectId, sf::FloatRect bounds, PartColor color);
    bool tryCollect(const sf::FloatRect& playerBounds, ProgressModel& progress) override;
    [[nodiscard]] bool isActive() const override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    [[nodiscard]] sf::Color getColor() const;

    std::string m_objectId;
    sf::FloatRect m_bounds;
    PartColor m_color;
    bool m_active{true};
};
