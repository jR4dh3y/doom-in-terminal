#include "game.hpp"
#include <stdexcept>
#include <iostream>

int main() {
    try {
        Game game;
        game.init();
        game.run();
        game.cleanup();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}