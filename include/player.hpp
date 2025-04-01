#pragma once

#include <cmath>

class Player {
public:
    Player(float startX = 0.0f, float startY = 0.0f);

    void move(float dx, float dy);
    void rotate(float angle);
    void update(float deltaTime);

    float getX() const { return x; }
    float getY() const { return y; }
    float getAngle() const { return angle; }
    float getDirX() const { return dirX; }
    float getDirY() const { return dirY; }
    float getPlaneX() const { return planeX; }
    float getPlaneY() const { return planeY; }

private:
    float x, y;        // Player position
    float angle;       // Player viewing angle
    float dirX, dirY;  // Direction vector
    float planeX, planeY; // Camera plane
    float moveSpeed;
    float rotSpeed;
};