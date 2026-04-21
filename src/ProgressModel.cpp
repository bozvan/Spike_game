#include "ProgressModel.hpp"

#include <algorithm>

void ProgressModel::addObserver(IProgressObserver& observer)
{
    const auto it = std::find(m_observers.begin(), m_observers.end(), &observer);
    if (it == m_observers.end())
    {
        m_observers.push_back(&observer);
        observer.onProgressChanged(*this);
    }
}

void ProgressModel::removeObserver(IProgressObserver& observer)
{
    m_observers.erase(
        std::remove(m_observers.begin(), m_observers.end(), &observer),
        m_observers.end());
}

void ProgressModel::reset()
{
    m_collectedObjects.clear();
    m_collectedKeysByLevel.clear();
    m_requiredKeysByLevel.clear();
    m_collectedParts = {false, false, false};
    m_currentLevelId.clear();
    m_lives = 3;
    notifyObservers();
}

void ProgressModel::setCurrentLevel(std::string levelId)
{
    m_currentLevelId = std::move(levelId);
    notifyObservers();
}

void ProgressModel::setLevelKeyTarget(const std::size_t totalKeys)
{
    m_requiredKeysByLevel[m_currentLevelId] = totalKeys;
    notifyObservers();
}

const std::string& ProgressModel::getCurrentLevel() const
{
    return m_currentLevelId;
}

int ProgressModel::getLives() const
{
    return m_lives;
}

std::size_t ProgressModel::getCollectedKeysInCurrentLevel() const
{
    const auto it = m_collectedKeysByLevel.find(m_currentLevelId);
    return it == m_collectedKeysByLevel.end() ? 0U : it->second;
}

std::size_t ProgressModel::getRequiredKeysInCurrentLevel() const
{
    const auto it = m_requiredKeysByLevel.find(m_currentLevelId);
    return it == m_requiredKeysByLevel.end() ? 0U : it->second;
}

bool ProgressModel::areAllKeysCollectedInCurrentLevel() const
{
    const std::size_t required = getRequiredKeysInCurrentLevel();
    return required == 0U || getCollectedKeysInCurrentLevel() >= required;
}

std::size_t ProgressModel::getCollectedKeyCount() const
{
    std::size_t totalKeys = 0U;
    for (const auto& [levelId, collectedKeys] : m_collectedKeysByLevel)
    {
        static_cast<void>(levelId);
        totalKeys += collectedKeys;
    }

    return totalKeys;
}

std::size_t ProgressModel::getRequiredKeyCount() const
{
    std::size_t totalKeys = 0U;
    for (const auto& [levelId, requiredKeys] : m_requiredKeysByLevel)
    {
        static_cast<void>(levelId);
        totalKeys += requiredKeys;
    }

    return totalKeys;
}

bool ProgressModel::hasCollectedObject(const std::string& objectId) const
{
    return m_collectedObjects.find(objectId) != m_collectedObjects.end();
}

bool ProgressModel::hasPart(const PartColor color) const
{
    return m_collectedParts[colorToIndex(color)];
}

std::size_t ProgressModel::getCollectedPartCount() const
{
    return static_cast<std::size_t>(std::count(m_collectedParts.begin(), m_collectedParts.end(), true));
}

std::size_t ProgressModel::getRequiredPartCount() const
{
    return m_collectedParts.size();
}

bool ProgressModel::areAllKeysCollected() const
{
    const std::size_t requiredKeys = getRequiredKeyCount();
    return requiredKeys > 0U && getCollectedKeyCount() >= requiredKeys;
}

bool ProgressModel::areAllPartsCollected() const
{
    return getCollectedPartCount() >= getRequiredPartCount();
}

bool ProgressModel::isFullyCompleted() const
{
    return areAllKeysCollected() && areAllPartsCollected();
}

bool ProgressModel::collectKey(const std::string& objectId)
{
    if (!markCollected(objectId))
    {
        return false;
    }

    ++m_collectedKeysByLevel[m_currentLevelId];
    notifyObservers();
    return true;
}

bool ProgressModel::collectPart(const std::string& objectId, const PartColor color)
{
    if (!markCollected(objectId))
    {
        return false;
    }

    m_collectedParts[colorToIndex(color)] = true;
    notifyObservers();
    return true;
}

bool ProgressModel::markCollected(const std::string& objectId)
{
    return m_collectedObjects.insert(objectId).second;
}

bool ProgressModel::loseLife()
{
    if (m_lives <= 0)
    {
        return false;
    }

    --m_lives;
    notifyObservers();
    return m_lives == 0;
}

void ProgressModel::restoreLives(const int lives)
{
    m_lives = lives;
    notifyObservers();
}

void ProgressModel::notifyObservers() const
{
    for (IProgressObserver* observer : m_observers)
    {
        if (observer != nullptr)
        {
            observer->onProgressChanged(*this);
        }
    }
}

std::size_t ProgressModel::colorToIndex(const PartColor color)
{
    return static_cast<std::size_t>(color);
}
