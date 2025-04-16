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
    
    // Mini-map properties
    static const int MINIMAP_WIDTH = 24;
    static const int MINIMAP_HEIGHT = 24;
    void renderMiniMap(const Player& player, const Map& map);
    void drawPlayerOnMiniMap(const Player& player);

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
    float* zBuffer;  // Store wall distances
};