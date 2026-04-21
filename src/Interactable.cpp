#include "Interactable.hpp"

#include <iostream>
#include <utility>

DoorInteractable::DoorInteractable(std::string objectId,
                                   const sf::FloatRect bounds,
                                   std::string targetLevelId,
                                   std::string targetSpawnName,
                                   const bool requiresAllKeys)
    : m_objectId(std::move(objectId)),
      m_bounds(bounds),
      m_targetLevelId(std::move(targetLevelId)),
      m_targetSpawnName(std::move(targetSpawnName)),
      m_requiresAllKeys(requiresAllKeys)
{
}

void DoorInteractable::syncState(const sf::FloatRect& playerBounds, const ProgressModel& progress)
{
    m_playerNearby = playerBounds.findIntersection(m_bounds).has_value();
    m_unlocked = !m_requiresAllKeys || progress.areAllKeysCollectedInCurrentLevel();
}

std::optional<LevelTransitionRequest> DoorInteractable::tryInteract(const bool interactRequested,
                                                                    const ProgressModel& progress)
{
    if (!interactRequested || !m_playerNearby)
    {
        return std::nullopt;
    }

    if (m_requiresAllKeys && !progress.areAllKeysCollectedInCurrentLevel())
    {
        std::cout << "Door is locked. Keys: "
                  << progress.getCollectedKeysInCurrentLevel()
                  << '/'
                  << progress.getRequiredKeysInCurrentLevel()
                  << '\n';
        return std::nullopt;
    }

    std::cout << "Door opened: " << m_objectId << '\n';
    return LevelTransitionRequest{m_targetLevelId, m_targetSpawnName};
}

void DoorInteractable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape door(m_bounds.size);
    door.setPosition(m_bounds.position);
    door.setFillColor(m_unlocked ? sf::Color(96, 174, 89) : sf::Color(104, 62, 32));
    door.setOutlineThickness(3.f);
    door.setOutlineColor(m_playerNearby ? sf::Color(255, 231, 122) : sf::Color(50, 30, 18));
    target.draw(door, states);

    sf::CircleShape knob(4.f);
    knob.setFillColor(sf::Color(235, 192, 61));
    knob.setPosition({
        m_bounds.position.x + m_bounds.size.x - 14.f,
        m_bounds.position.y + m_bounds.size.y * 0.5f});
    target.draw(knob, states);
}

ElevatorInteractable::ElevatorInteractable(std::string objectId,
                                           const sf::FloatRect bounds,
                                           std::string targetLevelId,
                                           std::string targetSpawnName)
    : m_objectId(std::move(objectId)),
      m_bounds(bounds),
      m_targetLevelId(std::move(targetLevelId)),
      m_targetSpawnName(std::move(targetSpawnName))
{
}

void ElevatorInteractable::syncState(const sf::FloatRect& playerBounds, const ProgressModel&)
{
    m_playerNearby = playerBounds.findIntersection(m_bounds).has_value();
}

std::optional<LevelTransitionRequest> ElevatorInteractable::tryInteract(const bool interactRequested,
                                                                        const ProgressModel&)
{
    if (!interactRequested || !m_playerNearby)
    {
        return std::nullopt;
    }

    std::cout << "Elevator activated: " << m_objectId << '\n';
    return LevelTransitionRequest{m_targetLevelId, m_targetSpawnName};
}

void ElevatorInteractable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    sf::RectangleShape frame(m_bounds.size);
    frame.setPosition(m_bounds.position);
    frame.setFillColor(sf::Color(90, 98, 118));
    frame.setOutlineThickness(3.f);
    frame.setOutlineColor(m_playerNearby ? sf::Color(255, 231, 122) : sf::Color(40, 44, 55));
    target.draw(frame, states);

    sf::RectangleShape cabin({m_bounds.size.x - 12.f, m_bounds.size.y - 12.f});
    cabin.setPosition({m_bounds.position.x + 6.f, m_bounds.position.y + 6.f});
    cabin.setFillColor(sf::Color(163, 181, 214));
    target.draw(cabin, states);
}
