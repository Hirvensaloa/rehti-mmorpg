#include "graphics.h"
#include <iostream>
int main() {
	testFunc(); // should be available in test.hpp. That in turn includes glfw library
	std::cout << "Winner winner chicken dinner" << std::endl;
}