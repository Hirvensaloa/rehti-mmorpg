#pragma once

struct Coordinates
{
    Coordinates(int x0 = 0, int y0 = 0, int z0 = 0) : x(x0), y(y0), z(z0) {}

    int x;
    int y;
    int z;

    bool operator==(Coordinates &other)
    {
        return (x == other.x && y == other.y && z == other.z);
    }
};