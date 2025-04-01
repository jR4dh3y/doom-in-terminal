#pragma once

#include <vector>
#include <string>

class Map {
public:
    Map(int width = 24, int height = 24);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCell(int x, int y) const;
    char getWallTexture(int x, int y) const;
    bool isWall(int x, int y) const;

    void loadMap(const std::string& filename);
    void setCell(int x, int y, int value);

private:
    int width;
    int height;
    std::vector<std::vector<int>> worldMap;
    std::vector<std::vector<char>> wallTextures;

    void initializeDefaultMap();
    void initializeWallTextures();
};