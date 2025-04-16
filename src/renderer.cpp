#include "renderer.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

Renderer::Renderer(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), zBuffer(nullptr) {
    buffer.resize(screenHeight, std::string(screenWidth, ' '));
    
    // Initialize wall texture with single pattern
    textures.resize(1); // Use only one wall type
    textures[0].resize(TEX_WIDTH * TEX_HEIGHT);
    
    // Initialize zBuffer
    zBuffer = new float[screenWidth];

    // Create brick pattern for the single wall texture
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            bool isBrick = ((y / 8) % 2 == 0 && (x / 16 + y / 8) % 2 == 0) ||
                          ((y / 8) % 2 == 1 && (x / 16 + y / 8 + 1) % 2 == 0);
            textures[0][y * TEX_WIDTH + x] = isBrick ? '#' : '=';
        }
    }
}

Renderer::~Renderer() {
    cleanup();
    delete[] zBuffer;
    zBuffer = nullptr;
}

void Renderer::init() {
    // Initialize any renderer-specific resources
}

void Renderer::cleanup() {
    // Cleanup any renderer-specific resources
}

char Renderer::getShaderChar(float distance) const {
    // Convert distance to shader character based on intensity
    const int numShaders = strlen(shaderChars);
    int index = std::min(static_cast<int>(distance * 2.0f), numShaders - 1);
    return shaderChars[index];
}

void Renderer::calculateRay(const Player& player, int x, float& rayDirX, float& rayDirY) {
    // Calculate ray position and direction
    float cameraX = 2.0f * x / static_cast<float>(screenWidth) - 1.0f;
    rayDirX = player.getDirX() + player.getPlaneX() * cameraX;
    rayDirY = player.getDirY() + player.getPlaneY() * cameraX;
}

void Renderer::performDDA(const Map& map, float rayDirX, float rayDirY, float startX, float startY, Renderer::RaycastHit& hit) {
    // Initialize DDA parameters
    int mapX = static_cast<int>(startX);
    int mapY = static_cast<int>(startY);

    // Calculate step and initial sideDist
    float deltaDistX = std::abs(1.0f / rayDirX);
    float deltaDistY = std::abs(1.0f / rayDirY);

    int stepX = (rayDirX < 0) ? -1 : 1;
    int stepY = (rayDirY < 0) ? -1 : 1;

    float sideDistX = (rayDirX < 0) ? (startX - mapX) * deltaDistX : (mapX + 1.0f - startX) * deltaDistX;
    float sideDistY = (rayDirY < 0) ? (startY - mapY) * deltaDistY : (mapY + 1.0f - startY) * deltaDistY;
    //Perform DDA
    hit.hit = false;
    hit.side = 0;
    while (!hit.hit) {
        // Jump to next square
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            hit.side = 0;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            hit.side = 1;
        }

        // Check if ray has hit a wall
        if (map.isWall(mapX, mapY)) {
            hit.hit = true;
            hit.mapX = mapX;
            hit.mapY = mapY;
        }
    }

    //Calculate distance and exact wall hit position
    if (hit.side == 0) {
        hit.distance = (mapX - startX + (1 - stepX) / 2) / rayDirX;
        hit.wallX = startY + hit.distance * rayDirY;
    } else {
        hit.distance = (mapY - startY + (1 - stepY) / 2) / rayDirY;
        hit.wallX = startX + hit.distance * rayDirX;
    }
    hit.wallX -= floor(hit.wallX);

    // Calculate texture coordinates
    hit.texX = static_cast<int>(hit.wallX * static_cast<float>(TEX_WIDTH));
    if (hit.side == 0 && rayDirX > 0) hit.texX = TEX_WIDTH - hit.texX - 1;
    if (hit.side == 1 && rayDirY < 0) hit.texX = TEX_WIDTH - hit.texX - 1;
}

void Renderer::renderMinimap(const Player& player, const Map& map) {
    const int minimapSize = 12;
    const int minimapX = screenWidth - minimapSize - 2;
    const int minimapY = 2;

    // Draw minimap border
    for (int y = 0; y < minimapSize + 2; y++) {
        for (int x = 0; x < minimapSize + 2; x++) {
            if (y == 0 || y == minimapSize + 1 || x == 0 || x == minimapSize + 1) {
                mvaddch(minimapY + y, minimapX + x, '+');
            }
        }
    }

    // Draw map and player
    for (int y = 0; y < minimapSize; y++) {
        for (int x = 0; x < minimapSize; x++) {
            int mapX = static_cast<int>(player.getX()) + x - minimapSize/2;
            int mapY = static_cast<int>(player.getY()) + y - minimapSize/2;
            
            if (mapX >= 0 && mapX < map.getWidth() && mapY >= 0 && mapY < map.getHeight()) {
                char ch = map.isWall(mapX, mapY) ? '□' : ' ';
                mvaddch(minimapY + 1 + y, minimapX + 1 + x, ch);
            }
        }
    }

    // Draw player direction indicator
    int playerScreenX = minimapX + 1 + minimapSize/2;
    int playerScreenY = minimapY + 1 + minimapSize/2;
    char dirChar = '>';
    float angle = atan2(player.getDirY(), player.getDirX());
    if (angle < -2.748f || angle > 2.748f) dirChar = '>';
    else if (angle < -0.393f) dirChar = '^';
    else if (angle < 1.963f) dirChar = '<';
    else if (angle < 2.748f) dirChar = 'v';
    mvaddch(playerScreenY, playerScreenX, dirChar);
}

void Renderer::render(const Player& player, const Map& map) {
    for (int x = 0; x < screenWidth; x++) {
        // Calculate ray direction
        float rayDirX, rayDirY;
        calculateRay(player, x, rayDirX, rayDirY);

        // Perform DDA and get wall hit information
        Renderer::RaycastHit hit;
        performDDA(map, rayDirX, rayDirY, player.getX(), player.getY(), hit);

        // Store wall distance
        zBuffer[x] = hit.distance;

        // Calculate wall height and drawing bounds
        int lineHeight = static_cast<int>(screenHeight / hit.distance);
        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

        // Draw the textured wall slice
        for (int y = drawStart; y <= drawEnd; y++) {
            float step = 1.0f * TEX_HEIGHT / lineHeight;
            float texPos = (y - drawStart) * step;
            hit.texY = static_cast<int>(texPos) & (TEX_HEIGHT - 1);

            char texel = textures[0][TEX_WIDTH * hit.texY + hit.texX];
            if (hit.side == 1) texel = getShaderChar(std::min(hit.distance * 1.5f, 8.0f));
            buffer[y][x] = texel;
        }

        // Draw floor and ceiling
        for (int y = 0; y < drawStart; y++) buffer[y][x] = '.';
        for (int y = drawEnd + 1; y < screenHeight; y++) buffer[y][x] = ' ';
    }

    // Draw the game view buffer to screen
    for (int y = 0; y < screenHeight; y++) {
        mvaddstr(y, 0, buffer[y].c_str());
    }

    // Render minimap
    renderMinimap(player, map);
}