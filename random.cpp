#include "random.h"
#include <cmath>
#include <algorithm>

uint32_t random::procGen;

double random::getDouble(double min, double max) {
	return ((double)random::rnd() / (double)(0xFFFFFFFF)) * (max - min) + min;
}

double random::getDouble() {
	return (double)random::rnd();
}

void random::seed(int x, int y) {
	random::procGen = (x & 0xFFFF) << 16 | (y & 0xFFFF);
}

int random::getInt(int min, int max) {
	return (rnd() % (max - min)) + min;
}

int random::getInt() {
	return rnd();
}

uint32_t random::rnd() {	
		procGen += 0xe120fc15;
		uint64_t tmp;
		tmp = (uint64_t)procGen * 0x4a39b70d;
		uint32_t m1 = (tmp >> 32) ^ tmp;
		tmp = (uint64_t)m1 * 0x12fad5c9;
		uint32_t m2 = (tmp >> 32) ^ tmp;
		return m2;
}

/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 *
 * as an alternative, this slightly faster equivalent function (macro) can be used:
 * #define lerp(a0, a1, w) ((a0) + (w)*((a1) - (a0))) 
 */
 
 
#define PI 3.1415927;

float perlin::noise(int x, int y)
{
    int n = x + y * 57;
    n = (n<<13) ^ n;
    return (1.0 - ( (n * ((n * n * 15731) + 789221) +  1376312589) & 0x7fffffff) / 1073741824.0);
}

float perlin::cosine_interpolate(float a, float b, float x)
{
    float ft = x * PI
    float f = (1 - cos(ft)) * 0.5;
    float result =  a*(1-f) + b*f;
    return result;
}

float perlin::smooth_noise_2D(float x, float y)
{  
    float corners = ( noise(x-1, y-1)+noise(x+1, y-1)+noise(x-1, y+1)+noise(x+1, y+1) ) / 16;
    float sides   = ( noise(x-1, y)  +noise(x+1, y)  +noise(x, y-1)  +noise(x, y+1) ) /  8;
    float center  =  noise(x, y) / 4;

    return corners + sides + center;
}

float perlin::interpolated_noise(float x, float y)
{
    int x_whole = (int) x;
    float x_frac = x - x_whole;

    int y_whole = (int) y;
    float y_frac = y - y_whole;

    float v1 = smooth_noise_2D(x_whole, y_whole); 
    float v2 = smooth_noise_2D(x_whole, y_whole+1); 
    float v3 = smooth_noise_2D(x_whole+1, y_whole); 
    float v4 = smooth_noise_2D(x_whole+1, y_whole+1); 

    float i1 = cosine_interpolate(v1,v3,x_frac);
    float i2 = cosine_interpolate(v2,v4,x_frac);

    return cosine_interpolate(i1, i2, y_frac);
}

float perlin::octaves = 8.0f;
float perlin::persistence = 0.5f;
float perlin::lacunarity = 1.0f;

float perlin::getPerlin(float x, float y, int octaves, float persistence) {
    float total = 0;

    for(int i=0; i<octaves-1; i++)
    {
        float frequency = pow(2,i);
        float amplitude = pow(persistence,i);
        total = total + interpolated_noise(x * frequency, y * frequency) * amplitude;
    }
    return total;
}

float perlin::getPerlin(float x, float y)
{
	//const float octaves = 8.0f;
	//const float persistence = 0.5f;
	
    float total = 0;

    for(int i=0; i<octaves-1; i++)
    {
        float frequency = pow(2,i);
        float amplitude = pow(persistence,i);
        total = total + interpolated_noise(x * frequency, y * frequency) * amplitude;
    }
    return total;
}
 
float perlin::getNormal(float omax, float omin, float max, float min, float value) {
	return (max - min) / (omax - omin) * (value - omax) + max;
}

float perlin::getNormalNoise(float x, float z) {
	float noise = perlin::getPerlin(x,z);
	return perlin::getNormal(1,-1,1,0,noise);
}
 
 /*
perlin::constructor perlin::cons;

float perlin::Gradient[100][100][2];

perlin::constructor::constructor() {
	perlin::_construct();
}

perlin::constructor::~constructor() {
	perlin::_destruct();
}
 
float perlin::lerp(float a0, float a1, float w) {
    return (1.0f - w)*a0 + w*a1;
}

// Computes the dot product of the distance and gradient vectors.
float perlin::dotGridGradient(int ix, int iy, float x, float y) {

    // Precomputed (or otherwise) gradient vectors at each grid node
    //const float Gradient[100][100][2];

    // Compute the distance vector
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // Compute the dot-product
    return (dx*perlin::Gradient[iy][ix][0] + dy*perlin::Gradient[iy][ix][1]);
}

// Compute Perlin noise at coordinates x, y
float perlin::getPerlin(float x, float y) {

    // Determine grid cell coordinates
    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = lerp(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = lerp(n0, n1, sx);

    value = lerp(ix0, ix1, sy);
    return value;
}
*/