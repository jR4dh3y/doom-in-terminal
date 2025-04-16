#pragma once

#include <ncurses.h>
#include <vector>
#include <string>
#include "player.hpp"
#include "map.hpp"

class Renderer {
public:
    struct RaycastHit {
        float distance;
        int mapX, mapY;
        bool hit;
        int side;
        float wallX;      // Where exactly the wall was hit
        int texX, texY;   // Texture coordinates
    };

    Renderer(int screenWidth, int screenHeight);
    ~Renderer();

    void init();
    void render(const Player& player, const Map& map);
    void renderMinimap(const Player& player, const Map& map);
    void cleanup();

    void calculateRay(const Player& player, int x, float& rayDirX, float& rayDirY);
    void performDDA(const Map& map, float rayDirX, float rayDirY, float startX, float startY, RaycastHit& hit);
    char getShaderChar(float distance) const;

    static const int TEX_WIDTH = 64;
    static const int TEX_HEIGHT = 64;

private:
    int screenWidth;
    int screenHeight;
    std::vector<std::string> buffer;
    const char shaderChars[12] = " .:-=+*#%@";
    std::vector<std::vector<char>> textures;
    float* zBuffer;
};