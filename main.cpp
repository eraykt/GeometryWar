#include <SFML/Graphics.hpp>

#include "Game.h"

// Use GPU, not integrated.
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C" {
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main() {
    Game g("config.txt");
    g.run();

    return 0;
}
 