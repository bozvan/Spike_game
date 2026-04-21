#pragma once

#include "ProgressModel.hpp"

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>

struct LevelTransitionRequest
{
    std::string targetLevelId;
    std::string targetSpawnName;
};

inline constexpr char kVictoryLevelId[] = "__victory__";

class Interactable : public sf::Drawable
{
public:
    virtual ~Interactable() = default;
    virtual void syncState(const sf::FloatRect& playerBounds, const ProgressModel& progress) = 0;
    [[nodiscard]] virtual std::optional<LevelTransitionRequest> tryInteract(bool interactRequested,
                                                                            const ProgressModel& progress) = 0;
};

class DoorInteractable final : public Interactable
{
public:
    DoorInteractable(std::string objectId,
                     sf::FloatRect bounds,
                     std::string targetLevelId,
                     std::string targetSpawnName,
                     bool requiresAllKeys);

    void syncState(const sf::FloatRect& playerBounds, const ProgressModel& progress) override;
    [[nodiscard]] std::optional<LevelTransitionRequest> tryInteract(bool interactRequested,
                                                                    const ProgressModel& progress) override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::string m_objectId;
    sf::FloatRect m_bounds;
    std::string m_targetLevelId;
    std::string m_targetSpawnName;
    bool m_requiresAllKeys{true};
    bool m_playerNearby{};
    bool m_unlocked{};
};

class ElevatorInteractable final : public Interactable
{
public:
    ElevatorInteractable(std::string objectId,
                         sf::FloatRect bounds,
                         std::string targetLevelId,
                         std::string targetSpawnName,
                         std::size_t requiredKeyCount);

    void syncState(const sf::FloatRect& playerBounds, const ProgressModel& progress) override;
    [[nodiscard]] std::optional<LevelTransitionRequest> tryInteract(bool interactRequested,
                                                                    const ProgressModel& progress) override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::string m_objectId;
    sf::FloatRect m_bounds;
    std::string m_targetLevelId;
    std::string m_targetSpawnName;
    std::size_t m_requiredKeyCount{};
    bool m_playerNearby{};
    bool m_unlocked{};
};

class MegaDoorInteractable final : public Interactable
{
public:
    MegaDoorInteractable(std::string objectId,
                         sf::FloatRect bounds,
                         std::string targetLevelId,
                         std::string targetSpawnName);

    void syncState(const sf::FloatRect& playerBounds, const ProgressModel& progress) override;
    [[nodiscard]] std::optional<LevelTransitionRequest> tryInteract(bool interactRequested,
                                                                    const ProgressModel& progress) override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::string m_objectId;
    sf::FloatRect m_bounds;
    std::string m_targetLevelId;
    std::string m_targetSpawnName;
    bool m_playerNearby{};
    bool m_unlocked{};
};
