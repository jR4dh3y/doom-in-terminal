#pragma once

#include <ncurses.h>
#include <vector>
#include <string>
#include "player.hpp"
#include "map.hpp"

class Renderer {
public:
    // Forward declare the struct
    struct RaycastHit;
    
    Renderer(int screenWidth, int screenHeight);
    ~Renderer();

    void init();
    void render(const Player& player, const Map& map);
    void cleanup();

public:
    void calculateRay(const Player& player, int x, float& rayDirX, float& rayDirY);
    void performDDA(const Map& map, float rayDirX, float rayDirY, float startX, float startY, RaycastHit& hit);
    char getShaderChar(float distance) const;

    int screenWidth;
    int screenHeight;
    std::vector<std::string> buffer;
    const char shaderChars[12] = " .:-=+*#%@";

    struct Sprite {
        float x, y;      // Position
        int texNum;      // Texture number
        float distance;  // Distance to player
    };

    struct RaycastHit {
        float distance;
        int mapX, mapY;
        bool hit;
        int side;
        float wallX;      // Where exactly the wall was hit
        int texNum;       // Which texture to use
        int texX, texY;   // Texture coordinates
    };

    static const int TEX_WIDTH = 64;
    static const int TEX_HEIGHT = 64;
    std::vector<std::vector<char>> textures;  // Store wall textures
    std::vector<std::vector<char>> spriteTextures;  // Store sprite textures
    std::vector<Sprite> sprites;  // Store sprite instances

    void renderSprites(const Player& player);
    void sortSprites(const Player& player);
    float* zBuffer;  // Store wall distances for sprite rendering
};