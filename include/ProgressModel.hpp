#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class PartColor
{
    Red = 0,
    Blue = 1,
    Green = 2
};

class ProgressModel;

class IProgressObserver
{
public:
    virtual ~IProgressObserver() = default;
    virtual void onProgressChanged(const ProgressModel& progress) = 0;
};

class ProgressModel
{
public:
    // Observer pattern: the progress model notifies UI and other systems
    // whenever lives or collected objects change.
    void addObserver(IProgressObserver& observer);
    void removeObserver(IProgressObserver& observer);

    void reset();
    void setCurrentLevel(std::string levelId);
    void setLevelKeyTarget(std::size_t totalKeys);

    [[nodiscard]] const std::string& getCurrentLevel() const;
    [[nodiscard]] int getLives() const;
    [[nodiscard]] std::size_t getCollectedKeysInCurrentLevel() const;
    [[nodiscard]] std::size_t getRequiredKeysInCurrentLevel() const;
    [[nodiscard]] bool areAllKeysCollectedInCurrentLevel() const;
    [[nodiscard]] std::size_t getCollectedKeyCount() const;
    [[nodiscard]] std::size_t getRequiredKeyCount() const;
    [[nodiscard]] bool hasCollectedObject(const std::string& objectId) const;
    [[nodiscard]] bool hasPart(PartColor color) const;
    [[nodiscard]] std::size_t getCollectedPartCount() const;
    [[nodiscard]] std::size_t getRequiredPartCount() const;
    [[nodiscard]] bool areAllKeysCollected() const;
    [[nodiscard]] bool areAllPartsCollected() const;
    [[nodiscard]] bool isFullyCompleted() const;

    bool collectKey(const std::string& objectId);
    bool collectPart(const std::string& objectId, PartColor color);
    bool markCollected(const std::string& objectId);
    bool loseLife();
    void restoreLives(int lives);

private:
    void notifyObservers() const;
    static std::size_t colorToIndex(PartColor color);

    std::vector<IProgressObserver*> m_observers;
    std::unordered_set<std::string> m_collectedObjects;
    std::unordered_map<std::string, std::size_t> m_collectedKeysByLevel;
    std::unordered_map<std::string, std::size_t> m_requiredKeysByLevel;
    std::array<bool, 3> m_collectedParts{};
    std::string m_currentLevelId;
    int m_lives{3};
};
