#pragma once

#include <ncurses.h>
#include <memory>
#include <vector>
#include "player.hpp"
#include "map.hpp"
#include "renderer.hpp"

class Game {
public:
    Game();
    ~Game();

    void init();
    void run();
    void cleanup();

private:
    void processInput();
    void update();
    void render();

    std::unique_ptr<Player> player;
    std::unique_ptr<Map> map;
    std::unique_ptr<Renderer> renderer;
    bool isRunning;
    int screenWidth;
    int screenHeight;
};