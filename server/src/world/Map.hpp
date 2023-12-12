#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

#include "Coordinates.hpp"
#include "RehtiReader.hpp"

/**
 * @brief Static class representing the map of the game world. Map contains information about which tiles are accessible and which are not.
 * It also contains information about the height of the tiles. It does not contain any information about the entities in the world.
 * For that refer to GameWorld.
 */
class Map
{
public:
    /**
     * @brief Loads the map information from generated assets. Currently loads the access map and height map information.
     * @note Call this before using any other methods of this class.
     * @returns Nothing
     */
    static void loadMap();

    /**
     * @brief Finds a path from start to end
     * @param start Starting coordinates
     * @param end Ending coordinates
     * @returns Vector of coordinates (x,y) that form the path. Empty vector if no path found.
     */
    static std::vector<std::pair<int, int>> findPath(Coordinates start, Coordinates end);

    /**
     * @brief Gives the height for the given (x, y) coordinates
     * @param x X-coordinate
     * @param y Y-coordinate
     * @returns Height of the given coordinates wrapped in optional.
     */
    static std::optional<int> getHeight(int x, int y);

    /**
     * @brief Get the access map
     *
     * @return access map
     */
    static const std::vector<std::vector<uint8_t>>& getAccessMap();

    /**
     * @brief Gets a random coordinate within the given bounds.
     *
     * @param coordinateBounds
     * @return Coordinates
     */
    static Coordinates getRandomCoordinates(const SpawnCoordinateBounds& coordinateBounds);

private:
    inline static std::vector<std::vector<uint8_t>> accessMapM = {};
    inline static std::vector<std::vector<int>> heightMapM = {};
};
