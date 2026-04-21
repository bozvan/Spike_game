#pragma once

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace tinyxml2
{
class XMLElement;
}

class MapLoader : public sf::Drawable
{
public:
    struct MapObject
    {
        int id{};
        std::string name;
        std::string type;
        sf::FloatRect bounds;
        std::unordered_map<std::string, std::string> properties;
    };

    bool loadFromFile(const std::string& mapPath);
    void update(float deltaTimeSeconds);

    [[nodiscard]] sf::Vector2u getMapSizeInTiles() const;
    [[nodiscard]] sf::Vector2u getTileSize() const;
    [[nodiscard]] sf::Vector2f getWorldSize() const;
    [[nodiscard]] const std::vector<MapObject>& getObjects(const std::string& layerName) const;
    [[nodiscard]] sf::Vector2f getPlayerSpawn() const;
    [[nodiscard]] const MapObject* findObject(const std::string& layerName, const std::string& objectName) const;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    struct AnimationFrame
    {
        std::uint32_t localTileId{};
        float durationSeconds{};
    };

    struct Tileset
    {
        std::uint32_t firstGid{};
        std::uint32_t tileWidth{};
        std::uint32_t tileHeight{};
        std::uint32_t tileCount{};
        std::uint32_t columns{};
        std::filesystem::path imagePath;
        sf::Texture texture;
        std::unordered_map<std::uint32_t, std::vector<AnimationFrame>> animations;
    };

    struct AnimatedTileInstance
    {
        std::size_t vertexOffset{};
        std::vector<AnimationFrame> frames;
        std::size_t currentFrameIndex{};
        float elapsedTime{};
    };

    struct LayerBatch
    {
        std::size_t tilesetIndex{};
        sf::VertexArray vertices{sf::PrimitiveType::Triangles};
        std::vector<AnimatedTileInstance> animatedTiles;
    };

    struct TileLayer
    {
        std::string name;
        bool visible{true};
        std::uint32_t width{};
        std::uint32_t height{};
        std::vector<std::uint32_t> gids;
        std::vector<LayerBatch> batches;
    };

    struct ObjectLayer
    {
        std::string name;
        bool visible{true};
        std::vector<MapObject> objects;
    };

    void clear();
    bool loadTilesetFromElement(const tinyxml2::XMLElement* tilesetElement,
                                const std::filesystem::path& baseDirectory);
    bool parseTileLayer(const tinyxml2::XMLElement* layerElement);
    void parseObjectLayer(const tinyxml2::XMLElement* objectLayerElement);
    void buildLayerGeometry(TileLayer& layer);
    void appendTileVertices(LayerBatch& batch,
                            std::uint32_t gid,
                            sf::Vector2f worldPosition);
    void applyTileTextureCoords(sf::VertexArray& vertices,
                                std::size_t vertexOffset,
                                const Tileset& tileset,
                                std::uint32_t localTileId) const;
    [[nodiscard]] std::optional<std::size_t> findTilesetIndexForGid(std::uint32_t gid) const;

    std::filesystem::path m_mapPath;
    sf::Vector2u m_mapSizeInTiles{};
    sf::Vector2u m_tileSize{};
    std::vector<Tileset> m_tilesets;
    std::vector<TileLayer> m_tileLayers;
    std::vector<ObjectLayer> m_objectLayers;
    std::unordered_map<std::string, std::size_t> m_objectLayerLookup;
};
