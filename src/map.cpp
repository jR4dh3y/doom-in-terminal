#include "map.hpp"
#include <random>

Map::Map(int width, int height) : width(width), height(height) {
    worldMap.resize(height, std::vector<int>(width, 0));
    wallTextures.resize(height, std::vector<char>(width, ' '));
    initializeDefaultMap();
    initializeWallTextures();
}

int Map::getCell(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return 1; // Out of bounds is wall
    return worldMap[y][x];
}

char Map::getWallTexture(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) return '#';
    return wallTextures[y][x];
}

bool Map::isWall(int x, int y) const {
    return getCell(x, y) != 0;
}

void Map::setCell(int x, int y, int value) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        worldMap[y][x] = value;
    }
}

void Map::initializeDefaultMap() {
    // 1  walls, 0  empty space)
    const int fixedMap[24][24] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1},
        {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1},
        {1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Copy map layout to worldMap
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            worldMap[y][x] = fixedMap[y][x];
        }
    }

    // player starting position is clear
    worldMap[12][12] = 0;
    worldMap[12][13] = 0;
    worldMap[13][12] = 0;
    worldMap[13][13] = 0;
}

void Map::initializeWallTextures() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (worldMap[y][x] == 1) {
                wallTextures[y][x] = '#';
            }
        }
    }
}

void Map::loadMap(const std::string& filename) {
    initializeDefaultMap();
    initializeWallTextures();
}