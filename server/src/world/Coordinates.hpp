#pragma once

struct Coordinates
{
    Coordinates(int x0 = 0, int y0 = 0);

    int x;
    int y;
    int z;

    bool operator==(Coordinates &other);

    unsigned int distance(const Coordinates &other);
};