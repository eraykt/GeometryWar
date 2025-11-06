#include <SFML/Graphics.hpp>
#include <iostream>
#include "Game.h"

// Use GPU, not integrated.
extern "C" {
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
extern "C" {
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{
	Game g("config.txt");

	std::cout << g.getRandomInt(1, 5);
	g.spawnEnemy();

	g.run();

	return 0;
}
