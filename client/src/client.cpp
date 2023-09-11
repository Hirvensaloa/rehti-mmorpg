#include "graphics.h"
#include "network.h"
#include <iostream>
int main() {
	testFunc(); // should be available in test.hpp. That in turn includes glfw library
	testNetwork();
	std::cout << "Winner winner chicken dinner" << std::endl;
}