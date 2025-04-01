#include "game.hpp"
#include <chrono>
#include <thread>
#include <stdexcept>

Game::Game() : isRunning(false), screenWidth(80), screenHeight(24) {
    player = std::make_unique<Player>(12.0f, 12.0f);
    map = std::make_unique<Map>();
    renderer = std::make_unique<Renderer>(screenWidth, screenHeight);
}

Game::~Game() {
    cleanup();
}

void Game::init() {
    // Initialize ncurses
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    timeout(0);

    // Initialize game components
    renderer->init();
    isRunning = true;
}

void Game::run() {
    const int targetFPS = 30;
    const std::chrono::milliseconds frameTime(1000 / targetFPS);

    while (isRunning) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        processInput();
        update();
        render();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        
        if (elapsedTime < frameTime) {
            std::this_thread::sleep_for(frameTime - elapsedTime);
        }
    }
}

void Game::processInput() {
    int ch = getch();
    switch (ch) {
        case 'w': player->move(0.1f, 0.0f); break;
        case 's': player->move(-0.1f, 0.0f); break;
        case 'a': player->rotate(-0.1f); break;
        case 'd': player->rotate(0.1f); break;
        case 'q': isRunning = false; break;
    }
}

void Game::update() {
    player->update(1.0f / 30.0f);
}

void Game::render() {
    clear();
    renderer->render(*player, *map);
    refresh();
}

void Game::cleanup() {
    if (isRunning) {
        renderer->cleanup();
        endwin();
        isRunning = false;
    }
}