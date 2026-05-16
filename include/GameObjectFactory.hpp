#pragma once

#include "Collectible.hpp"
#include "Enemy.hpp"
#include "Interactable.hpp"
#include "MapLoader.hpp"

#include <memory>
#include <string>

class ProgressModel;

class GameObjectFactory
{
public:
    // Factory Method pattern: converts generic TMX objects
    // Враг - создается по типу
    static std::unique_ptr<Enemy> createEnemy(const std::string& levelId, const MapLoader::MapObject& object); 
    
    // Берет данные с карты - создаение какого-то объекта (монеты) и сколько монет нужно
    static std::unique_ptr<Collectible> createCollectible(const std::string& levelId,const MapLoader::MapObject& object, const ProgressModel& progress);

    // Создает интерактивный объект - дверь. По входному признаку(типу)
    static std::unique_ptr<Interactable> createInteractable(const std::string& levelId, const MapLoader::MapObject& object);

    // уникальный ID для каждого объекта - сейвит предыдущие удаленные объекты
    static std::string makeObjectId(const std::string& levelId, const MapLoader::MapObject& object);
};
