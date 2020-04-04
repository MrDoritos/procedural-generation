#pragma once
#include <cstdint>
#include <cmath>

class random {
	public:
		static int getInt(int min, int max);
		static double getDouble(double min, double max);
		static uint32_t rnd();
		static void seed(int x, int y);
		static uint32_t procGen;
		static double getDouble();
		static int getInt();
};

class perlin {
	public:
		//static float lerp(float a0, float a1, float w);
		//static float dotGridGradient(int ix, int iy, float x, float y);
		//static float getPerlin(float x, float y);
		static float getPerlin(float x, float y);
		static float cosine_interpolate(float a, float b, float x);
		static float smooth_noise_2D(float x, float y);
		static float interpolated_noise(float x, float y);
		static float noise(int x, int y);
		
		static float octaves;
		static float persistence;
		
	private:
		/*
		friend class constructor;
		
		struct constructor {
			constructor();
			~constructor();
		};
		
		static constructor cons;
		
		static void _construct() {
			for (int x = 0; x < 100; x++) {
				for (int y = 0; y < 100; y++) {
					for (int i = 0; i < 2; i++) {
						perlin::Gradient[x][y][i] = (2 * random::rnd() - 1);
						perlin::Gradient[x][y][i] /= sqrtf(0xFFFFFFFF);
					}
				}
			}
		}

		static void _destruct() {
		
		}	
		
		static float Gradient[100][100][2];
		*/
};