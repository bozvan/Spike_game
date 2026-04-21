#include "MapLoader.hpp"

#include <tinyxml2.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <utility>

namespace
{
constexpr std::uint32_t flippedHorizontallyFlag = 0x80000000U;
constexpr std::uint32_t flippedVerticallyFlag = 0x40000000U;
constexpr std::uint32_t flippedDiagonallyFlag = 0x20000000U;
constexpr std::uint32_t gidMask = ~(flippedHorizontallyFlag | flippedVerticallyFlag | flippedDiagonallyFlag);

std::string trimCopy(const std::string& value)
{
    const auto first = std::find_if_not(
        value.begin(),
        value.end(),
        [](unsigned char ch)
        {
            return std::isspace(ch) != 0;
        });

    if (first == value.end())
    {
        return {};
    }

    const auto last = std::find_if_not(
        value.rbegin(),
        value.rend(),
        [](unsigned char ch)
        {
            return std::isspace(ch) != 0;
        }).base();

    return std::string(first, last);
}

std::filesystem::path resolvePath(const std::filesystem::path& baseDirectory, const std::string& relativePath)
{
    return (baseDirectory / std::filesystem::path(relativePath)).lexically_normal();
}

std::vector<std::uint32_t> parseCsvData(const char* csvText)
{
    std::vector<std::uint32_t> gids;

    if (csvText == nullptr)
    {
        return gids;
    }

    std::stringstream stream(csvText);
    std::string token;

    while (std::getline(stream, token, ','))
    {
        const std::string trimmed = trimCopy(token);
        if (trimmed.empty())
        {
            continue;
        }

        gids.push_back(static_cast<std::uint32_t>(std::strtoul(trimmed.c_str(), nullptr, 10)));
    }

    return gids;
}

std::string propertyValueFromElement(const tinyxml2::XMLElement* propertyElement)
{
    if (propertyElement == nullptr)
    {
        return {};
    }

    if (const char* value = propertyElement->Attribute("value"))
    {
        return value;
    }

    if (const char* text = propertyElement->GetText())
    {
        return text;
    }

    return {};
}
}

void MapLoader::clear()
{
    m_mapPath.clear();
    m_mapSizeInTiles = {};
    m_tileSize = {};
    m_tilesets.clear();
    m_tileLayers.clear();
    m_objectLayers.clear();
    m_objectLayerLookup.clear();
}

bool MapLoader::loadFromFile(const std::string& mapPath)
{
    clear();
    m_mapPath = std::filesystem::path(mapPath);

    tinyxml2::XMLDocument document;
    const tinyxml2::XMLError loadResult = document.LoadFile(mapPath.c_str());
    if (loadResult != tinyxml2::XML_SUCCESS)
    {
        std::cerr << "Failed to load TMX map: " << mapPath << '\n';
        return false;
    }

    const tinyxml2::XMLElement* mapElement = document.FirstChildElement("map");
    if (mapElement == nullptr)
    {
        std::cerr << "TMX file does not contain <map>: " << mapPath << '\n';
        return false;
    }

    const char* orientation = mapElement->Attribute("orientation");
    if (orientation != nullptr && std::string(orientation) != "orthogonal")
    {
        std::cerr << "Only orthogonal TMX maps are supported.\n";
        return false;
    }

    m_mapSizeInTiles.x = mapElement->UnsignedAttribute("width");
    m_mapSizeInTiles.y = mapElement->UnsignedAttribute("height");
    m_tileSize.x = mapElement->UnsignedAttribute("tilewidth");
    m_tileSize.y = mapElement->UnsignedAttribute("tileheight");

    const std::filesystem::path baseDirectory = m_mapPath.parent_path();

    for (const tinyxml2::XMLElement* child = mapElement->FirstChildElement();
         child != nullptr;
         child = child->NextSiblingElement())
    {
        const std::string nodeName = child->Name();

        if (nodeName == "tileset")
        {
            if (!loadTilesetFromElement(child, baseDirectory))
            {
                return false;
            }
        }
        else if (nodeName == "layer")
        {
            if (!parseTileLayer(child))
            {
                return false;
            }
        }
        else if (nodeName == "objectgroup")
        {
            parseObjectLayer(child);
        }
    }

    std::sort(
        m_tilesets.begin(),
        m_tilesets.end(),
        [](const Tileset& lhs, const Tileset& rhs)
        {
            return lhs.firstGid < rhs.firstGid;
        });

    for (TileLayer& layer : m_tileLayers)
    {
        buildLayerGeometry(layer);
    }

    return true;
}

bool MapLoader::loadTilesetFromElement(const tinyxml2::XMLElement* tilesetElement,
                                       const std::filesystem::path& baseDirectory)
{
    tinyxml2::XMLDocument externalDocument;

    const tinyxml2::XMLElement* tilesetRoot = tilesetElement;
    std::filesystem::path tilesetBaseDirectory = baseDirectory;
    const std::uint32_t firstGid = tilesetElement->UnsignedAttribute("firstgid");

    if (const char* externalSource = tilesetElement->Attribute("source"))
    {
        const std::filesystem::path tsxPath = resolvePath(baseDirectory, externalSource);
        if (externalDocument.LoadFile(tsxPath.string().c_str()) != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Failed to load TSX tileset: " << tsxPath.string() << '\n';
            return false;
        }

        tilesetRoot = externalDocument.FirstChildElement("tileset");
        if (tilesetRoot == nullptr)
        {
            std::cerr << "TSX file does not contain <tileset>: " << tsxPath.string() << '\n';
            return false;
        }

        tilesetBaseDirectory = tsxPath.parent_path();
    }

    if (tilesetRoot == nullptr)
    {
        return false;
    }

    Tileset tileset;
    tileset.firstGid = firstGid;
    tileset.tileWidth = tilesetRoot->UnsignedAttribute("tilewidth", m_tileSize.x);
    tileset.tileHeight = tilesetRoot->UnsignedAttribute("tileheight", m_tileSize.y);
    tileset.tileCount = tilesetRoot->UnsignedAttribute("tilecount");
    tileset.columns = tilesetRoot->UnsignedAttribute("columns");

    const tinyxml2::XMLElement* imageElement = tilesetRoot->FirstChildElement("image");
    if (imageElement == nullptr || imageElement->Attribute("source") == nullptr)
    {
        std::cerr << "Tileset image source is missing.\n";
        return false;
    }

    tileset.imagePath = resolvePath(tilesetBaseDirectory, imageElement->Attribute("source"));

    if (!tileset.texture.loadFromFile(tileset.imagePath.string()))
    {
        std::cerr << "Failed to load tileset image: " << tileset.imagePath.string() << '\n';
        return false;
    }

    tileset.texture.setSmooth(false);

    if (tileset.columns == 0 && tileset.tileWidth != 0)
    {
        tileset.columns = tileset.texture.getSize().x / tileset.tileWidth;
    }

    for (const tinyxml2::XMLElement* tileElement = tilesetRoot->FirstChildElement("tile");
         tileElement != nullptr;
         tileElement = tileElement->NextSiblingElement("tile"))
    {
        const std::uint32_t tileId = tileElement->UnsignedAttribute("id");
        const tinyxml2::XMLElement* animationElement = tileElement->FirstChildElement("animation");

        if (animationElement == nullptr)
        {
            continue;
        }

        std::vector<AnimationFrame> frames;
        for (const tinyxml2::XMLElement* frameElement = animationElement->FirstChildElement("frame");
             frameElement != nullptr;
             frameElement = frameElement->NextSiblingElement("frame"))
        {
            AnimationFrame frame;
            frame.localTileId = frameElement->UnsignedAttribute("tileid");
            frame.durationSeconds = static_cast<float>(frameElement->UnsignedAttribute("duration", 100U)) / 1000.f;
            frames.push_back(frame);
        }

        if (!frames.empty())
        {
            tileset.animations.emplace(tileId, std::move(frames));
        }
    }

    m_tilesets.push_back(std::move(tileset));
    return true;
}

bool MapLoader::parseTileLayer(const tinyxml2::XMLElement* layerElement)
{
    TileLayer layer;
    layer.name = layerElement->Attribute("name") != nullptr ? layerElement->Attribute("name") : "";
    layer.visible = layerElement->BoolAttribute("visible", true);
    layer.width = layerElement->UnsignedAttribute("width", m_mapSizeInTiles.x);
    layer.height = layerElement->UnsignedAttribute("height", m_mapSizeInTiles.y);

    const tinyxml2::XMLElement* dataElement = layerElement->FirstChildElement("data");
    if (dataElement == nullptr)
    {
        std::cerr << "Tile layer '" << layer.name << "' does not contain <data>.\n";
        return false;
    }

    const char* encoding = dataElement->Attribute("encoding");
    if (encoding == nullptr || std::string(encoding) != "csv")
    {
        std::cerr << "Tile layer '" << layer.name << "' must use CSV encoding.\n";
        return false;
    }

    layer.gids = parseCsvData(dataElement->GetText());

    const std::size_t expectedTileCount = static_cast<std::size_t>(layer.width) * static_cast<std::size_t>(layer.height);
    if (layer.gids.size() != expectedTileCount)
    {
        std::cerr << "Tile layer '" << layer.name << "' has invalid CSV tile count.\n";
        return false;
    }

    m_tileLayers.push_back(std::move(layer));
    return true;
}

void MapLoader::parseObjectLayer(const tinyxml2::XMLElement* objectLayerElement)
{
    ObjectLayer layer;
    layer.name = objectLayerElement->Attribute("name") != nullptr ? objectLayerElement->Attribute("name") : "";
    layer.visible = objectLayerElement->BoolAttribute("visible", true);

    for (const tinyxml2::XMLElement* objectElement = objectLayerElement->FirstChildElement("object");
         objectElement != nullptr;
         objectElement = objectElement->NextSiblingElement("object"))
    {
        MapObject object;
        object.id = objectElement->IntAttribute("id");
        object.name = objectElement->Attribute("name") != nullptr ? objectElement->Attribute("name") : "";
        object.type = objectElement->Attribute("type") != nullptr ? objectElement->Attribute("type") : "";
        object.bounds.position.x = objectElement->FloatAttribute("x");
        object.bounds.position.y = objectElement->FloatAttribute("y");
        object.bounds.size.x = objectElement->FloatAttribute("width");
        object.bounds.size.y = objectElement->FloatAttribute("height");

        if (const tinyxml2::XMLElement* propertiesElement = objectElement->FirstChildElement("properties"))
        {
            for (const tinyxml2::XMLElement* propertyElement = propertiesElement->FirstChildElement("property");
                 propertyElement != nullptr;
                 propertyElement = propertyElement->NextSiblingElement("property"))
            {
                const char* propertyName = propertyElement->Attribute("name");
                if (propertyName == nullptr)
                {
                    continue;
                }

                object.properties.emplace(propertyName, propertyValueFromElement(propertyElement));
            }
        }

        layer.objects.push_back(std::move(object));
    }

    m_objectLayerLookup[layer.name] = m_objectLayers.size();
    m_objectLayers.push_back(std::move(layer));
}

void MapLoader::buildLayerGeometry(TileLayer& layer)
{
    layer.batches.clear();

    std::unordered_map<std::size_t, std::size_t> batchLookup;

    for (std::uint32_t y = 0; y < layer.height; ++y)
    {
        for (std::uint32_t x = 0; x < layer.width; ++x)
        {
            const std::size_t tileIndex = static_cast<std::size_t>(y) * layer.width + x;
            const std::uint32_t gid = layer.gids[tileIndex] & gidMask;

            if (gid == 0)
            {
                continue;
            }

            const std::optional<std::size_t> tilesetIndex = findTilesetIndexForGid(gid);
            if (!tilesetIndex.has_value())
            {
                continue;
            }

            std::size_t batchIndex{};
            const auto existingBatch = batchLookup.find(*tilesetIndex);
            if (existingBatch == batchLookup.end())
            {
                batchIndex = layer.batches.size();
                batchLookup.emplace(*tilesetIndex, batchIndex);
                layer.batches.push_back({});
                layer.batches.back().tilesetIndex = *tilesetIndex;
            }
            else
            {
                batchIndex = existingBatch->second;
            }

            appendTileVertices(
                layer.batches[batchIndex],
                gid,
                {static_cast<float>(x * m_tileSize.x), static_cast<float>(y * m_tileSize.y)});
        }
    }
}

void MapLoader::appendTileVertices(LayerBatch& batch,
                                   const std::uint32_t gid,
                                   const sf::Vector2f worldPosition)
{
    const Tileset& tileset = m_tilesets[batch.tilesetIndex];
    const std::uint32_t localTileId = gid - tileset.firstGid;
    const std::size_t vertexOffset = batch.vertices.getVertexCount();

    const sf::Vector2f topLeft = worldPosition;
    const sf::Vector2f topRight{worldPosition.x + static_cast<float>(m_tileSize.x), worldPosition.y};
    const sf::Vector2f bottomRight{
        worldPosition.x + static_cast<float>(m_tileSize.x),
        worldPosition.y + static_cast<float>(m_tileSize.y)};
    const sf::Vector2f bottomLeft{worldPosition.x, worldPosition.y + static_cast<float>(m_tileSize.y)};

    for (int index = 0; index < 6; ++index)
    {
        batch.vertices.append({});
    }

    batch.vertices[vertexOffset + 0].position = topLeft;
    batch.vertices[vertexOffset + 1].position = bottomLeft;
    batch.vertices[vertexOffset + 2].position = bottomRight;
    batch.vertices[vertexOffset + 3].position = topLeft;
    batch.vertices[vertexOffset + 4].position = bottomRight;
    batch.vertices[vertexOffset + 5].position = topRight;

    const auto animationIt = tileset.animations.find(localTileId);
    if (animationIt != tileset.animations.end())
    {
        batch.animatedTiles.push_back({vertexOffset, animationIt->second, 0U, 0.f});
        applyTileTextureCoords(batch.vertices, vertexOffset, tileset, animationIt->second.front().localTileId);
    }
    else
    {
        applyTileTextureCoords(batch.vertices, vertexOffset, tileset, localTileId);
    }
}

void MapLoader::applyTileTextureCoords(sf::VertexArray& vertices,
                                       const std::size_t vertexOffset,
                                       const Tileset& tileset,
                                       const std::uint32_t localTileId) const
{
    if (tileset.columns == 0)
    {
        return;
    }

    const std::uint32_t column = localTileId % tileset.columns;
    const std::uint32_t row = localTileId / tileset.columns;

    const float left = static_cast<float>(column * tileset.tileWidth);
    const float top = static_cast<float>(row * tileset.tileHeight);
    const float right = left + static_cast<float>(tileset.tileWidth);
    const float bottom = top + static_cast<float>(tileset.tileHeight);

    vertices[vertexOffset + 0].texCoords = {left, top};
    vertices[vertexOffset + 1].texCoords = {left, bottom};
    vertices[vertexOffset + 2].texCoords = {right, bottom};
    vertices[vertexOffset + 3].texCoords = {left, top};
    vertices[vertexOffset + 4].texCoords = {right, bottom};
    vertices[vertexOffset + 5].texCoords = {right, top};
}

std::optional<std::size_t> MapLoader::findTilesetIndexForGid(const std::uint32_t gid) const
{
    std::optional<std::size_t> bestIndex;

    for (std::size_t index = 0; index < m_tilesets.size(); ++index)
    {
        if (m_tilesets[index].firstGid <= gid)
        {
            bestIndex = index;
        }
        else
        {
            break;
        }
    }

    return bestIndex;
}

void MapLoader::update(const float deltaTimeSeconds)
{
    for (TileLayer& layer : m_tileLayers)
    {
        for (LayerBatch& batch : layer.batches)
        {
            const Tileset& tileset = m_tilesets[batch.tilesetIndex];

            for (AnimatedTileInstance& animatedTile : batch.animatedTiles)
            {
                if (animatedTile.frames.empty())
                {
                    continue;
                }

                animatedTile.elapsedTime += deltaTimeSeconds;

                while (animatedTile.elapsedTime >= animatedTile.frames[animatedTile.currentFrameIndex].durationSeconds)
                {
                    if (animatedTile.frames[animatedTile.currentFrameIndex].durationSeconds <= 0.f)
                    {
                        break;
                    }

                    animatedTile.elapsedTime -= animatedTile.frames[animatedTile.currentFrameIndex].durationSeconds;
                    animatedTile.currentFrameIndex =
                        (animatedTile.currentFrameIndex + 1U) % animatedTile.frames.size();

                    applyTileTextureCoords(
                        batch.vertices,
                        animatedTile.vertexOffset,
                        tileset,
                        animatedTile.frames[animatedTile.currentFrameIndex].localTileId);
                }
            }
        }
    }
}

sf::Vector2u MapLoader::getMapSizeInTiles() const
{
    return m_mapSizeInTiles;
}

sf::Vector2u MapLoader::getTileSize() const
{
    return m_tileSize;
}

sf::Vector2f MapLoader::getWorldSize() const
{
    return {
        static_cast<float>(m_mapSizeInTiles.x * m_tileSize.x),
        static_cast<float>(m_mapSizeInTiles.y * m_tileSize.y)};
}

const std::vector<MapLoader::MapObject>& MapLoader::getObjects(const std::string& layerName) const
{
    static const std::vector<MapObject> emptyObjects;

    const auto it = m_objectLayerLookup.find(layerName);
    if (it == m_objectLayerLookup.end())
    {
        return emptyObjects;
    }

    return m_objectLayers[it->second].objects;
}

sf::Vector2f MapLoader::getPlayerSpawn() const
{
    for (const ObjectLayer& layer : m_objectLayers)
    {
        for (const MapObject& object : layer.objects)
        {
            if (object.type == "PlayerSpawn" || object.name == "PlayerSpawn")
            {
                return object.bounds.position;
            }
        }
    }

    return {static_cast<float>(m_tileSize.x), static_cast<float>(m_tileSize.y)};
}

const MapLoader::MapObject* MapLoader::findObject(const std::string& layerName, const std::string& objectName) const
{
    const auto& objects = getObjects(layerName);
    const auto it = std::find_if(
        objects.begin(),
        objects.end(),
        [&objectName](const MapObject& object)
        {
            return object.name == objectName;
        });

    if (it == objects.end())
    {
        return nullptr;
    }

    return &(*it);
}

void MapLoader::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const TileLayer& layer : m_tileLayers)
    {
        if (!layer.visible)
        {
            continue;
        }

        for (const LayerBatch& batch : layer.batches)
        {
            states.texture = &m_tilesets[batch.tilesetIndex].texture;
            target.draw(batch.vertices, states);
        }
    }
}
