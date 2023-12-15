#pragma once

/**
 * @brief Struct for coordinates
 */
struct Coordinates
{
    /**
     * @brief Construct a new Coordinates object. Z value is fetched based on the x and y values.
     *
     * @param x0
     * @param y0
     */
    Coordinates(int x0 = 0, int y0 = 0);

    int x;
    int y;
    int z;

    /**
     * @brief Check if coordinates match
     *
     * @param other Coordinates
     * @return bool
     */
    bool operator==(Coordinates& other);

    /**
     * @brief Compare coordinates
     *
     * @param l Coordinates
     * @param r Coordinates
     * @return true if l < r. false if l >= r.
     */
    friend bool operator<(const Coordinates& l, const Coordinates& r);

    /**
     * @brief Calculate the euclidean distance between two Coordinates
     *
     * @param other Coordinates
     * @return distance in integers
     */
    unsigned int distance(const Coordinates& other);
};