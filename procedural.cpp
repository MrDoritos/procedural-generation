#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <cmath>
#include <string.h>
#include <random>

//#include "lodepng.h"
#include "console.h"
//#include "png.h"

#include "random.h"
#include "terrain.h"
//#include "terrain3d.h"
//#include "galaxy.h"

using namespace std;

int main(int argc, char** argv) {
	terrain terra;
	//terrain3d terra;
	terra.loop();
	//while (console::readKey() != 'q');
	//random::seed(0,232);
	//fprintf(stderr, "\r\n%f\r\n", float(random::getDouble(1.0d, 255.0d)));
	//console::sleep(-1);
	return 0;
	
	//Galaxy.h
	/*
	int x, y;
	if (argc > 2 && sscanf(argv[1], "%i", &x) == 1 && sscanf(argv[2], "%i", &y) == 1) {
		fprintf(stderr, "(%i,%i)", x, y);
		(new program(x, y))->loop();
	} else {
		//Generate a random number
		x = rand();
		y = rand();
		fprintf(stderr, "(%i,%i)", x, y);
		(new program())->loop();
	}
		
	return 0;
	*/
}
