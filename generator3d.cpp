#include <stdio.h>

#include "generator3d.h"
#include "random.h"
#include "console.h"

void generator3d::generateBlockAt(int x, int y, int z, char& character, char& color) {
	//3d
	double ab = generator3d::getNormalPerlin(x, y);
	double bc = generator3d::getNormalPerlin(y, z);
	double ac = generator3d::getNormalPerlin(x, z);
	
	double ba = generator3d::getNormalPerlin(y, x);
	double cb = generator3d::getNormalPerlin(z, y);
	double ca = generator3d::getNormalPerlin(z, x);
	
	double abc = ab + bc + ac + ba + cb + ca;
	abc /= 6.0d;
	
	generator3d::mapNoise(generator3d::getNormal(abc), character, color);
	
	//generator3d::mapNoise(generator3d::getNormal(perlin::getPerlin(z, generator3d::getNormal(perlin::getPerlin(x, y))))	, character, color);
}

void generator3d::mapNoise(double normal, char& character, char& color) {
	const char v[] = { "         .:-=+%*#@" };
	character = v[int(floor(normal * 12.5d))];
	color = FWHITE | BBLACK;
	
	//fprintf(stderr, "%hf", normal);
}

double generator3d::getNormal(double value) {
	return (value + 1.0d)/(1.0d + 1.0d) * (1.0d);
}

double generator3d::getNormalPerlin(double x, double y) {
	return generator3d::getNormal(perlin::getPerlin(x, y));
}

double generator3d::getNormalPerlin3d(double x, double y, double z) {
	
}