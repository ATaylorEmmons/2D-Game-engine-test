#pragma once


//A global variable for debugging convenience
//	will be removed after clean up
//Static for method local to a file
#define Local static

//Static for variable defined in a scope
#define Persistent static

//Static for a global variable
#define Global static

#include <stdint.h>

#include <iostream>



void debug_printMsg(std::string msg) {
	std::cout << msg << std::endl;
}

void debug_printFloat(float f) {
	std::cout << '\r';
	std::cout.precision(5);
	std::cout << f;
}