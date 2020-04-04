#pragma once
#include <string>
#include <bits/stdc++.h>
#include "random.h"

class generator3d {
	public:
		static void generateBlockAt(int x, int y, int z, char& character, char& color);
		
	private:
		static void mapNoise(double normal, char& character, char& color);
		static double getNormal(double value); // For -1:1 range only
		static double getRange(double normal, double min, double max);
		static double getNormalPerlin(double x, double y);
		static double getNormalPerlin3d(double x, double y, double z);
};