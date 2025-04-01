#include "player.hpp"
#include <cmath>

Player::Player(float startX, float startY) : 
    x(startX), y(startY),
    angle(0.0f),
    dirX(1.0f), dirY(0.0f),
    planeX(0.0f), planeY(0.66f),
    moveSpeed(0.05f),
    rotSpeed(0.03f) {}

void Player::move(float dx, float dy) {
    float newX = x + dirX * dx - dirY * dy;
    float newY = y + dirY * dx + dirX * dy;

    // Simple collision detection (can be improved)
    if (newX >= 1.0f && newX < 23.0f) x = newX;
    if (newY >= 1.0f && newY < 23.0f) y = newY;
}

void Player::rotate(float angle) {
    float oldDirX = dirX;
    float oldPlaneX = planeX;

    // Rotate direction vector
    dirX = dirX * cos(angle) - dirY * sin(angle);
    dirY = oldDirX * sin(angle) + dirY * cos(angle);

    // Rotate camera plane
    planeX = planeX * cos(angle) - planeY * sin(angle);
    planeY = oldPlaneX * sin(angle) + planeY * cos(angle);
}

void Player::update(float deltaTime) {
    // Update player state based on deltaTime
    moveSpeed = 5.0f * deltaTime;
    rotSpeed = 3.0f * deltaTime;
}