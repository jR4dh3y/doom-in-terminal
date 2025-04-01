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
    // Create outer walls
    for (int x = 0; x < width; x++) {
        worldMap[0][x] = 1;
        worldMap[height-1][x] = 1;
    }
    for (int y = 0; y < height; y++) {
        worldMap[y][0] = 1;
        worldMap[y][width-1] = 1;
    }

    // Add some random walls inside
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 100);

    for (int y = 1; y < height-1; y++) {
        for (int x = 1; x < width-1; x++) {
            if (dis(gen) < 20) { // 20% chance for a wall
                worldMap[y][x] = 1;
            }
        }
    }

    // Ensure player starting position is clear
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
    // TODO: Implement map loading from file
    // For now, just use the default map
    initializeDefaultMap();
    initializeWallTextures();
}