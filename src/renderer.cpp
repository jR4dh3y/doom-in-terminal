#include "renderer.hpp"
#include <cmath>
#include <algorithm>
#include <cstring>

Renderer::Renderer(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight) {
    buffer.resize(screenHeight, std::string(screenWidth, ' '));
    
    // Initialize wall textures with different patterns
    textures.resize(4); // Support 4 different wall types
    for (auto& texture : textures) {
        texture.resize(TEX_WIDTH * TEX_HEIGHT);
    }

    // Initialize sprite textures
    spriteTextures.resize(2); // Support enemy and item sprites
    for (auto& texture : spriteTextures) {
        texture.resize(TEX_WIDTH * TEX_HEIGHT);
    }

    // Create enemy sprite pattern (skull-like)
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            bool isSkull = (x >= TEX_WIDTH/4 && x < 3*TEX_WIDTH/4 && 
                           y >= TEX_HEIGHT/4 && y < 3*TEX_HEIGHT/4) ||
                          ((x-TEX_WIDTH/2)*(x-TEX_WIDTH/2) + (y-TEX_HEIGHT/2)*(y-TEX_HEIGHT/2) < 100);
            spriteTextures[0][y * TEX_WIDTH + x] = isSkull ? '@' : ' ';
        }
    }

    // Create item sprite pattern (star-like)
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            bool isStar = abs(x - TEX_WIDTH/2) + abs(y - TEX_HEIGHT/2) < TEX_WIDTH/4 ||
                         abs(x - TEX_WIDTH/2) * 2 + abs(y - TEX_HEIGHT/2) < TEX_WIDTH/4;
            spriteTextures[1][y * TEX_WIDTH + x] = isStar ? '*' : ' ';
        }
    }

    // Create brick pattern for texture 0
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            bool isBrick = ((y / 8) % 2 == 0 && (x / 16 + y / 8) % 2 == 0) ||
                          ((y / 8) % 2 == 1 && (x / 16 + y / 8 + 1) % 2 == 0);
            textures[0][y * TEX_WIDTH + x] = isBrick ? '#' : '=';
        }
    }

    // Create vertical lines pattern for texture 1
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            textures[1][y * TEX_WIDTH + x] = (x % 8 == 0) ? '|' : ':';
        }
    }

    // Create checkerboard pattern for texture 2
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            textures[2][y * TEX_WIDTH + x] = ((x / 8 + y / 8) % 2 == 0) ? '+' : '*';
        }
    }

    // Create diagonal pattern for texture 3
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            textures[3][y * TEX_WIDTH + x] = ((x + y) % 8 == 0) ? '/' : '.';
        }
    }
}

Renderer::~Renderer() {
    cleanup();
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

    for (int x = 0; x < screenWidth; x++) {
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
            // Calculate texture Y coordinate
            float step = 1.0f * TEX_HEIGHT / lineHeight;
            float texPos = (y - drawStart) * step;
            hit.texY = static_cast<int>(texPos) & (TEX_HEIGHT - 1);

            // Get texture pixel and apply shading based on distance and side
            char texel = textures[hit.texNum][TEX_WIDTH * hit.texY + hit.texX];
            if (hit.side == 1) texel = getShaderChar(std::min(hit.distance * 1.5f, 8.0f));
            buffer[y][x] = texel;
        }

        // Draw floor and ceiling
        for (int y = 0; y < drawStart; y++) {
            buffer[y][x] = '.';
        }
        for (int y = drawEnd + 1; y < screenHeight; y++) {
            buffer[y][x] = ' ';
        }
    }

    // Render sprites after walls
    renderSprites(player);

    // Draw the buffer to screen
    for (int y = 0; y < screenHeight; y++) {
        mvaddstr(y, 0, buffer[y].c_str());
    }
}


void Renderer::sortSprites(const Player& player) {
    // Calculate distances
    for (auto& sprite : sprites) {
        float dx = sprite.x - player.getX();
        float dy = sprite.y - player.getY();
        sprite.distance = dx * dx + dy * dy;
    }
    // Sort sprites from far to close
    std::sort(sprites.begin(), sprites.end(),
        [](const Sprite& a, const Sprite& b) { return a.distance > b.distance; });
}
void Renderer::renderSprites(const Player& player) {
    sortSprites(player);

    for (const auto& sprite : sprites) {
        // Translate sprite position relative to camera
        float spriteX = sprite.x - player.getX();
        float spriteY = sprite.y - player.getY();

        // Transform sprite with the inverse camera matrix
        float invDet = 1.0f / (player.getPlaneX() * player.getDirY() - player.getDirX() * player.getPlaneY());
        float transformX = invDet * (player.getDirY() * spriteX - player.getDirX() * spriteY);
        float transformY = invDet * (-player.getPlaneY() * spriteX + player.getPlaneX() * spriteY);

        int spriteScreenX = static_cast<int>((screenWidth / 2) * (1 + transformX / transformY));

        // Calculate sprite dimensions on screen
        int spriteHeight = std::abs(static_cast<int>(screenHeight / transformY));
        int drawStartY = -spriteHeight / 2 + screenHeight / 2;
        if (drawStartY < 0) drawStartY = 0;
        int drawEndY = spriteHeight / 2 + screenHeight / 2;
        if (drawEndY >= screenHeight) drawEndY = screenHeight - 1;

        int spriteWidth = std::abs(static_cast<int>(screenHeight / transformY));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if (drawStartX < 0) drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if (drawEndX >= screenWidth) drawEndX = screenWidth - 1;

        // Render sprite columns
        for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
            if (transformY > 0 && stripe >= 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {
                for (int y = drawStartY; y < drawEndY; y++) {
                    int texY = ((y - drawStartY) * TEX_HEIGHT) / spriteHeight;
                    int texX = ((stripe - drawStartX) * TEX_WIDTH) / spriteWidth;
                    char texel = spriteTextures[sprite.texNum][texY * TEX_WIDTH + texX];
                    if (texel != ' ') buffer[y][stripe] = texel;
                }
            }
        }
    }
}