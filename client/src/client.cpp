#include "graphics.h"
#include "network.h"
#include <iostream>
int main() {
	RehtiGraphics* graphics = new RehtiGraphics();
	graphics->run();
	testNetwork();
	std::cout << "Winner winner chicken dinner" << std::endl;
}