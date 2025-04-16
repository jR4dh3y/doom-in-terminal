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
            hit.texNum = map.getCell(mapX, mapY) - 1; // Assuming wall types start from 1
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

void Renderer::render(const Player& player, const Map& map) {
    // Initialize zBuffer for sprite rendering
    if (!zBuffer) zBuffer = new float[screenWidth];

    // Split screen into two equal parts
    int gameViewWidth = screenWidth / 2; // Left half for game view

    for (int x = 0; x < gameViewWidth; x++) {
        // Calculate ray direction
        float rayDirX, rayDirY;
        calculateRay(player, x, rayDirX, rayDirY);

        // Perform DDA and get wall hit information
        Renderer::RaycastHit hit;
        performDDA(map, rayDirX, rayDirY, player.getX(), player.getY(), hit);

        // Store wall distance in zBuffer for sprite rendering
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

    // Draw the game view buffer to left half of screen
    for (int y = 0; y < screenHeight; y++) {
        mvaddstr(y, 0, buffer[y].substr(0, gameViewWidth).c_str());
    }

    // Draw the full-size map on right half with border
    int mapStartX = gameViewWidth;
    int mapDisplayWidth = screenWidth - gameViewWidth - 2; // Account for border
    int mapDisplayHeight = screenHeight - 2; // Account for border

    // Draw border
    for (int x = mapStartX; x < screenWidth; x++) {
        mvaddch(0, x, '-');
        mvaddch(screenHeight - 1, x, '-');
    }
    for (int y = 0; y < screenHeight; y++) {
        mvaddch(y, mapStartX, '|');
        mvaddch(y, screenWidth - 1, '|');
    }

    // Draw map contents
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            char tile = ' ';
            if (map.isWall(x, y)) {
                tile = '#'; // Use solid block for walls
            }
            int displayY = 1 + y * mapDisplayHeight / map.getHeight();
            int displayX = mapStartX + 1 + x * mapDisplayWidth / map.getWidth();
            
            // Ensure we don't write outside screen bounds
            if (displayY >= 1 && displayY < screenHeight - 1 && displayX >= mapStartX + 1 && displayX < screenWidth - 1) {
                mvaddch(displayY, displayX, tile);
            }
        }
    }

    // Draw player position on map with direction indicator
    int playerMapX = mapStartX + 1 + static_cast<int>(player.getX() * mapDisplayWidth / map.getWidth());
    int playerMapY = 1 + static_cast<int>(player.getY() * mapDisplayHeight / map.getHeight());
    
    // Ensure player marker is within screen bounds
    if (playerMapY >= 1 && playerMapY < screenHeight - 1 && playerMapX >= mapStartX + 1 && playerMapX < screenWidth - 1) {
        // Draw player direction indicator
        char playerChar;
        float dirX = player.getDirX();
        float dirY = player.getDirY();
        if (abs(dirX) > abs(dirY)) {
            playerChar = (dirX > 0) ? '>' : '<';
        } else {
            playerChar = (dirY > 0) ? 'v' : '^';
        }
        mvaddch(playerMapY, playerMapX, playerChar);
    }
}

void Renderer::renderMiniMap(const Player& player, const Map& map) {
    int startY = screenHeight - MINIMAP_HEIGHT - 1;
    int startX = (screenWidth - MINIMAP_WIDTH) / 2; // Center the mini-map horizontally
    
    // Draw mini-map border and contents
    for (int y = 0; y < MINIMAP_HEIGHT; y++) {
        for (int x = 0; x < MINIMAP_WIDTH; x++) {
            // Draw border
            if (y == 0 || y == MINIMAP_HEIGHT - 1) {
                buffer[startY + y][startX + x] = '-';
                continue;
            }
            if (x == 0 || x == MINIMAP_WIDTH - 1) {
                buffer[startY + y][startX + x] = '|';
                continue;
            }
            
            // Draw map contents
            int mapX = (x - 1) * (map.getWidth() - 2) / (MINIMAP_WIDTH - 2);
            int mapY = (y - 1) * (map.getHeight() - 2) / (MINIMAP_HEIGHT - 2);
            char tile = map.isWall(mapX, mapY) ? '#' : ' ';
            buffer[startY + y][startX + x] = tile;
        }
    }
}

void Renderer::drawPlayerOnMiniMap(const Player& player) {
    int startY = screenHeight - MINIMAP_HEIGHT - 1;
    int startX = (screenWidth - MINIMAP_WIDTH) / 2;
    int playerX = static_cast<int>(player.getX() * (MINIMAP_WIDTH - 2) / 24) + 1;
    int playerY = static_cast<int>(player.getY() * (MINIMAP_HEIGHT - 2) / 24) + 1;
    
    if (playerX >= 1 && playerX < MINIMAP_WIDTH - 1 && playerY >= 1 && playerY < MINIMAP_HEIGHT - 1) {
        // Determine player direction character based on direction vector
        char playerChar;
        float dirX = player.getDirX();
        float dirY = player.getDirY();
        
        // Use more precise direction detection
        float absX = std::abs(dirX);
        float absY = std::abs(dirY);
        
        if (absX > absY) {
            playerChar = (dirX > 0) ? '>' : '<';
        } else {
            playerChar = (dirY > 0) ? 'v' : '^';
        }
        
        buffer[startY + playerY][startX + playerX] = playerChar;
    }
}