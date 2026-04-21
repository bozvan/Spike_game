#pragma once

#include "ProgressModel.hpp"

#include <SFML/Graphics.hpp>

class HudOverlay : public sf::Drawable, public IProgressObserver
{
public:
    void onProgressChanged(const ProgressModel& progress) override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void drawHeart(sf::RenderTarget& target, sf::Vector2f position, bool filled) const;
    void drawPartIndicator(sf::RenderTarget& target, sf::Vector2f position, PartColor color, bool collected) const;
    void drawKeyIndicator(sf::RenderTarget& target, sf::Vector2f position, bool collected) const;
    [[nodiscard]] static sf::Color colorForPart(PartColor color);

    int m_lives{3};
    std::size_t m_keysCollected{};
    std::size_t m_keysRequired{};
    bool m_hasRedPart{};
    bool m_hasBluePart{};
    bool m_hasGreenPart{};
};
