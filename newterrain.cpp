#include "random.h"
#include "advancedConsole.h"
#include "PerlinNoise.h"
#include <math.h>
#include <vector>

struct state_t {
	float posX;
	float posZ;
	float scale;
	float actScale;
	
	int heightMapView;
} state;

struct distribution_t {
	virtual float distribute(float num);
};

struct perlin_redistribute_t : public distribution_t {
	float distribute(float num) override {
		auto clip = [](float num, float low, float high) {
			if (num > high) num = high;
			if (num < low) num = low;
			return num;
		};
		return clip(num, -0.5f, 0.5f);
		/*
		bool sign = num < 0;
		float norm = 1.0f - fabs(num);
		//n *= n;
		float n = pdf(norm);
		n = (0-n)+1;
		float y = 0.0f;
		y = n + num;
		clip(y, 0, 1);
		if (sign)
			return -y;
		else
			return y;
		//if (sign)
		//	return -n;
		//else
		//	return n;
		*/
	}
	float pdf(float x) {
		float u = 0.0f;
		float a = 0.25f;//0.5f;
		float x0 = x - u;
		x0 /= a;
		x0 *= x0;
		x0 *= -0.5f;
		
		float v0 = 1 / (a * sqrt(2 * 3.14));
		float e = pow(2.71828, x0);
		float y = (e * v0);
		y /= 2;
		
		//Simplify
		//  (2.718 ^ ((((x - u) / a)^2)*-.5)) * (1 / (a * sqrt(6.28)))
	}
} perlin_redistribute;

static siv::PerlinNoise noise;

struct random_t {
	random_t() {
		xscale = 0.1f;
		zscale = 0.1f;
		yscale = 50.0f;
		
		octaves = 3;
		persistence = 0.5f;
	}
	
	random_t(float xscale, float yscale, float zscale, float octaves, float persistence) {
		this->xscale = xscale;
		this->yscale = yscale;
		this->zscale = zscale;
		this->octaves = octaves;
		this->persistence = persistence;
	}
	
	float xscale;
	float zscale;
	float yscale;
	
	int octaves;
	float persistence;
};

struct climate_t {
	//Use large scale perlin
	float climatePerlinAt(int x, int y, int z) {
		float noise0 = perlin::getNormalNoise((x + 3000)/0.02f, (z + 3000)/0.02f);
		return noise0;
		//float noise1 = perlin::getNormalNoise(noise0 * ((((x + 3000) / 0.02f) + ((z + 3000) / 0.02f)) / 2), y);
		//return noise1;
	}	
	float temperatureAt(int x, int y, int z) {
		float climate = climatePerlinAt(x,y,z);
		float temperature = climate + (perlin::getPerlin(x/0.02f,z/0.02f));
		//2 - -1
		float tempNormal = perlin::getNormal(2,-1,1,0,temperature);
		return tempNormal;
	}
	float humidityAt(int x, int y, int z) {
		float climate = climatePerlinAt(x,y,z);
		float humidity = climate + (perlin::getPerlin((x+ 2000)/0.02f, (z + 2000)/0.02f));
		float humidityNormal = perlin::getNormal(2,-1,1,0,humidity);
		return humidityNormal;
	}
} climate;

struct biome_t {
	virtual void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) {
		ch = L'#';
		co = FBLACK | BWHITE;
	}
};
struct desert_b : biome_t { 
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '\\';
		co = BYELLOW | FWHITE;
	}
};
struct plains_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '_';
		co = BGREEN | FWHITE;
	}	
};
struct forest_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '&';
		co = BGREEN | FWHITE;
	}	
};
struct mountains_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '^';
		co = BBLACK | FWHITE | 0b10000000;
	}		
};
struct tundra_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '-';
		co = BWHITE | FWHITE;
	}		
};
struct tiaga_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '.';
		co = FMAGENTA | BWHITE | 0b10000000;
	}		
};
struct jungle_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '&';
		co = BGREEN | FWHITE | 0b10000000;
	}		
};
struct mesa_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '/';
		co = BRED | FYELLOW;
	}		
};
struct ice_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '/';
		co = BWHITE | FWHITE | 0b00001000;
	}		
};
struct lake_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '~';
		co = BBLUE | FCYAN;
	}			
};
struct river_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '~';
		co = BBLUE | FWHITE;
	}			
};
struct ocean_b : biome_t {
	void generate(int x, int z, float noise0, float noise1, wchar_t& ch, char& co) override {
		ch = '~';
		co = BBLUE | FWHITE | 0b10000000;
	}			
};

struct region_t {
	std::vector<biome_t*> biomeRegistry;
	
	virtual void generate(int x, int z, float noise0, wchar_t& ch, char& co) {
		//float noise1 = perlin_redistribute.distribute(perlin::getPerlin(x/6.8f,z/6.8f)) + 0.5f;
		float noise1 = perlin::getNormalNoise(x / 6.8f, z / 6.8f);
		biome_t* bio = biomeRegistry[(int)floorf(noise1 * biomeRegistry.size())];
		bio->generate(x, z, noise0, noise1, ch, co);
	}
};

struct newNoiseFunction_t {		 
	static float findMod(float a, float b) { 
		float mod; 
		// Handling negative values 
		if (a < 0) 
			mod = -a; 
		else
			mod =  a; 
		if (b < 0) 
			b = -b; 
	  
		// Finding mod by repeated subtraction 
		  
		while (mod >= b) 
			mod = mod - b; 
	  
		// Sign of result typically depends 
		// on sign of a. 
		if (a < 0) 
			return -mod; 
	  
		return mod; 
	} 
	
	static void mapValue(double value, float min, float max, wchar_t* ch, char* color) {
		if (value > max)		
			value = max;
		if (value < min)
			value = min;
		float normm = max - min;
		float normv = value - min;
		float norm = normv / normm;
		
		const wchar_t  clvls[] = { L' ', L'-', L'+', L'#', L'#', L'+', L'-', L' '  };//L" -+##+- ";
		const char colors[] = { FBLUE, FWHITE, FYELLOW, FGREEN, FGREEN };
		
		const int groups = sizeof(colors) - 1;
		
		int group = int(floor(norm * groups));
		char color0 = colors[group];
		char color1 = colors[group + 1];
		
		double normalizedGroupValue = norm / ((group + 1.0d) / groups);
		
		if (normalizedGroupValue >= 0.5d) {
			*color = color0 | (color1 << 4);
		} else {
			*color = color1 | (color0 << 4);
		}
		
		*ch = clvls[int(normalizedGroupValue * (sizeof(clvls) - 1))];			
	}
	
	#define c0f (FBLACK)
	#define c0b (BBLACK)
	#define c1f (FBLACK | 0b00001000)
	#define c1b (BBLACK | 0b10000000)
	#define c2f (FWHITE)
	#define c2b (BWHITE)
	#define c3f (FWHITE | 0b00001000)
	#define c3b (BWHITE | 0b10000000)

	static void getGradientGrayscale3(double value, double min, double max, wchar_t* ch, color_t* color) {
		const wchar_t clvls[] = L" ░▒░ ░▒░░";	
		const color_t colors[] = {
			c0f | c0b,
			c1f | c0b,
			c1f | c0b,
			c2f | c1b,
			c2f | c1b,
			c3f | c2b,
			c3f | c2b,
			c3f | c3b,
			c3f | c3b
		};
		
		const int clvlCount = 8;
		const int colorCount = 8;
		/*
		if (value > 1.0d)
			value = 1.0d;
		if (value < 0.0d)
			value = 0.0d;
		*/
		if (value > max)		
			value = max;
		if (value < min)
			value = min;
		float normm = max - min;
		float normv = value - min;
		float norm = normv / normm;
		
		*ch = clvls[int(norm * clvlCount)];
		*color = colors[int(norm * colorCount)];
	}
	
	static void colorize(float value, float min, float max, wchar_t *ch, char *co) {
		if (value > max)		
			value = max;
		if (value < min)
			value = min;
		float normm = max - min;
		float normv = value - min;
		float norm = normv / normm;
		if (norm < 0.6f && norm > 0.4f)
			*ch = L'░';//L'░';
		else
			*ch = L'░';//L'▒';
		
		/*
		if (norm > 0.90f)
			*co = FWHITE | BWHITE;
		else
		if (norm > 0.75f)
			*co = FWHITE | BBLACK;
		else
		if (norm > 0.50f)
			*co = FWHITE | BBLACK;
		else
		if (norm > 0.25f)
			*co = FBLACK | BWHITE;
		else
		if (norm > 0.10f)
			*co = FBLACK | BWHITE;
		else
			*co = FBLACK | BBLACK;
		*/
		
		char outofbound = FBLUE | BBLACK;
		const char colors[11] = {
			FBLUE | 0b00001000 | BBLACK,
			FBLUE | 0b00001000 | BBLUE,
			FBLUE | 0b00001000 | BCYAN,
			FCYAN | 0b00001000 | BBLUE,
			FCYAN | 0b00001000 | BGREEN,
			FGREEN | 0b00001000 | BCYAN,
			FGREEN | 0b00001000 | BYELLOW,
			FYELLOW | 0b00001000 | BGREEN,
			FYELLOW | 0b00001000 | BRED,
			FRED | 0b00001000 | BBLACK,
			FRED | BBLACK
		};
		
		*co = colors[int(floorf(norm * 10))];
		
		/*
		if (norm < 1.0f)
			*co = FWHITE | BWHITE;
		if (norm < 0.75f)
			*co = FBLACK | BWHITE;
		if (norm < 0.5f)
			*co = FWHITE | BBLACK;
		if (norm < 0.25f)
			*co = FBLACK | BBLACK;
		*/
		
	}
	
	static void haveFun(float x, float z, wchar_t *ch, char *co) {
		/*
		//float n = perlin::getPerlin(perlin::getPerlin(z,x), perlin::getPerlin(x,z));
		float n = perlin::getPerlin(x,z);
		if (n > 0.5f) { //Mountainous
			//Use actual perlin function, add stuff
			n += findMod(perlin::getNormal(1.0f, -1.0f, 5.0f, 0.0f, perlin::getPerlin(x,z, 4, 4)), 0.5f);
		} else
		if (n > 0.3f) { //Wavy
			n += sinf((x + z) * 0.2f) * 0.5f;
			if (n > 0.5f)
				n += perlin::getNormal(1.0f, -1.0f, 0.2f, -0.1f, perlin::getPerlin(n,z));
		}
		*/
		
		/* DO NOT CHANGE THIS IS COOL
		float temp = perlin::getPerlin(x * 0.1, z * 0.1);
		float humd = perlin::getPerlin(x * 0.2, z * temp);
		
		float climateCarve = temp + humd;
		*/
		
		float temp = perlin::getPerlin(x * 0.5, z * 0.4);
		float humd = perlin::getPerlin(x * 0.5, z * temp * 0.5);
		
		float airCirculation = sin(x * 0.5f) * 0.5f;//fabs(findMod(x * 0.5f, 2.0f) - 1.0f) - 0.5f;//sin(x*0.5);
		float humidityCirculation = sin(z * 0.25f) * 0.5f;
		
		float climateCarve = temp + humd * humidityCirculation;
		
		float heightMap = perlin::getPerlin(x,z);
		
		/*
		if (climateCarve > 1.0f || climateCarve < -1.0f) {
			heightMap -= 0.1f;
		}
		*/
		float seeClimate = (climateCarve + airCirculation) * 0.4f;
		heightMap += seeClimate;
		
		#define VIEW(x) getGradientGrayscale3(x, -1, 1, ch, (color_t*)co)
		
		switch (state.heightMapView) {
			case 1:
				VIEW(temp);
				break;
			case 2:
				VIEW(humd);
				break;
			case 3:
				VIEW(climateCarve);
				break;
			case 4:
				VIEW(airCirculation);
				break;
			case 5:
				VIEW(seeClimate);
				break;
			case 6:
				VIEW(humidityCirculation);
				break;
			default:
			case 0: 
				getGradientGrayscale3(heightMap,-0.49,0.54,ch,(color_t*)co);
				break;
		}
		//colorize(heightMap,-1.0,1.0,ch,co);
		//mapValue(heightMap,-1.0,1.0,ch,co);
		//getGradientGrayscale3(heightMap,-0.49,0.54,ch,(color_t*)co);
		//*co = FBLUE;
	}	
};

struct newTerrain_t {
	newTerrain_t() {
		
	}
	
	std::vector<region_t*> regionRegistry;
	std::vector<biome_t*> biomeRegistry;
	int climateRegistry[5][9] = {
		//COLD -> HOT
		/*WET*/	{11,10,9,6,5,4,3,2,0},
				{11,10,8,6,5,4,3,2,0},
				{11,10,9,6,5,4,3,1,0},
				{11,10,8,6,5,4,3,1,0},
		/*DRY*/	{11,10,10,6,5,4,3,1,0},
	};
	
	void landmassGenerate(float x, float z, float noise0, wchar_t& ch, char& co) {
		auto between = [](float _noise, float min, float max) {
			return (_noise >= min && _noise <= max);
		};
		
		float heightMap = perlin::getPerlin(x+5534,z+999,2,0.5f);
		float temp = (perlin::getPerlin(x,z,2,0.5f) + heightMap) / 2.0f;
		float humidity = (noise0 + perlin::getPerlin(x+345454,z+233,2,0.5f)) / 2.0f;
		if (between(temp, -1,-0.3))
			co = BWHITE;
		if (between(temp, -0.3, 0.3))
			co = BGREEN;
		if (between(temp, 0.3, 1))
			co = BYELLOW | 0b00001000;
		if (between(heightMap, 0.00f, 0.05f)) //Beach / Bank
			co = BBLUE;
		if (heightMap < 0.05f && noise0 < 0.05f)
			co = BYELLOW;
		if (between(humidity, -1,-0.3))
			ch = '^';
		if (between(humidity, -0.3,0.3))
			ch = '1';
		if (between(humidity, 0.3,1))
			ch = '#';
		
	}
	
	void generate(float x, float z, wchar_t& ch, char& co) {
		ch = ' ';
		co = FBLACK|BWHITE;
		float noise = perlin::getPerlin(x + perlin::getPerlin(x,z), z - perlin::getPerlin(x,z), 2, 0.5f);
		//noise += perlin::getPerlin(x + state.scale, z + state.scale, 2, 0.5f);
		//noise += perlin::getPerlin(x + state.scale, z - state.scale, 2, 0.5f);
		//noise += perlin::getPerlin(x - state.scale, z + state.scale, 2, 0.5f);
		//noise += perlin::getPerlin(x - state.scale, z - state.scale, 2, 0.5f);
		//noise /= 5;
		if (noise < 0.02f && noise > -0.02f)
			co = FBLACK|BBLUE;
		
		if (noise < -0.02f)
			biomeRegistry[5]->generate(x,z,noise,noise,ch,co);
		
		if (noise > 0.02f)
			landmassGenerate(x,z,noise,ch,co);
			//biomeRegistry[1]->generate(x,z,noise,noise,ch,co);
		
		/*
		//float noise = perlin_redistribute.distribute(perlin::getPerlin(x/state.scale,z/state.scale)) + 0.5f;
		float noise = perlin::getNormalNoise(x / state.scale,z / state.scale);
		region_t* reg = regionRegistry[(int)floorf(noise * regionRegistry.size())];
		reg->generate(x,z,noise,ch,co);
		*/
	}
	
	void generateClimate(int x, int z, wchar_t& ch, char& co) {
		float noise = perlin_redistribute.distribute(perlin::getPerlin(x/state.scale,z/state.scale)) + 0.5f;
		//perlin::getNormalNoise(x / state.scale, z / state.scale);
		//float temp = climate.temperatureAt(x / state.scale,60,z / state.scale);
		//float humd = climate.humidityAt(x / state.scale,60,z / state.scale);
		//biome_t* bio = biomeRegistry[climateRegistry[(int)floorf(humd*5.0f)][(int)floorf(temp*9)]];
		biome_t* bio = biomeRegistry[((int*)&climateRegistry)[(int)floorf(noise*45)]];
		bio->generate(x,z,noise,noise,ch,co);
	}
} newTerrain;

static bool showAltitude = false;

void handleInput(int key) {
	if (NOMOD(key) >= '0' && NOMOD(key <= '9'))
		state.heightMapView = NOMOD(key) - '0';
	switch (NOMOD(key)) {
		case 'w':
		case 'W':
			state.posZ -= 1 / (state.scale / 5);
			break;
		case 's':
		case 'S':
			state.posZ += 1 / (state.scale / 5);
			break;
		case 'a':
		case 'A':
			state.posX -= 1 / (state.scale / 5);
			break;
		case 'd':
		case 'D':
			state.posX += 1 / (state.scale / 5);
			break;
		case '+':
			state.actScale++;
			//state.scale += 0.1f;
			state.scale = pow(state.actScale, 1.5);
			break;
		case '-':			
			if (state.actScale - 1 < 1)
				state.actScale -= 0.1f;
			else
				state.actScale--;
			//state.scale -= 0.1f;
			state.scale = pow(state.actScale, 1.5);
			break;
		case '.':
			showAltitude = !showAltitude;
			break;
		default: break;
	}
}

int wmain() {
	//Plotting
	/*
	for (int x = 0; x < 990; x++) {
		printf("%i,%i\r\n", int(perlin_redistribute.distribute(perlin::getPerlin(x * 100, 0)) * 103.0f) + 100, int(x % 98));
	}	
	*/
	/*
	auto triangleWave = [](double x) {
		double a = 1.0d;
		double p = 1.0d;
		return ((2 * a) / 3.14) * asin(sin((6.28 / p) * x));
	};
	for (double x = 0; x < 99990; x++) {
		//printf("%i,%i\r\n", int((noise.accumulatedOctaveNoise2D(x * 100.123d, 100.456d, 8.0d) * 103.0d) + 100), int(x) % 98);
		printf("%i,%i\r\n", int(triangleWave(x) * 103.0d) + 500, int(x) % 98);
	}
	*/
	/*
	for (float x = -1.00f; x < 1.00f; x+=0.01f) {
		printf("%i,%i\r\n", int(x * 100.0f), 0-int(perlin_redistribute.pdf(x) * 100.0f));
	}
	*/
	//return 0;
	
	/*
	for (int x = 0; x < 5000; x+=1000)
		for (int y = 0; y < 5000; y+=1000)
			for (int z = 0; z < 5000; z+=1000)
			{
				fprintf(stderr, "%.2f:%.2f:%.2f\r\n", climate.temperatureAt(x,y,z), climate.humidityAt(x,y,z), climate.climatePerlinAt(x,y,z));
			}
	*/
	{
	state.scale = 6.4f;
	state.posX = 1000.0f;
	state.posZ = 1000.0f;
	
	desert_b desert;
	plains_b plain;
	forest_b forest;
	mountains_b mountain;
	river_b river;
	lake_b lake;
	ocean_b ocean;
	tundra_b tundra;
	tiaga_b tiaga;
	jungle_b jungle;
	mesa_b mesa;
	ice_b ice;
	
	region_t hot;
	region_t warm;
	region_t moderate;
	region_t cool;
	region_t cold;
	
	hot.biomeRegistry.push_back(&desert);
	hot.biomeRegistry.push_back(&mesa);
	warm.biomeRegistry.push_back(&plain);
	warm.biomeRegistry.push_back(&jungle);
	moderate.biomeRegistry.push_back(&mountain);
	moderate.biomeRegistry.push_back(&river);
	moderate.biomeRegistry.push_back(&lake);
	moderate.biomeRegistry.push_back(&ocean);
	moderate.biomeRegistry.push_back(&forest);
	cool.biomeRegistry.push_back(&tiaga);
	cold.biomeRegistry.push_back(&tundra);
	cold.biomeRegistry.push_back(&ice);
			
	newTerrain.biomeRegistry.push_back(&desert);
	newTerrain.biomeRegistry.push_back(&mesa);
	newTerrain.biomeRegistry.push_back(&plain);
	newTerrain.biomeRegistry.push_back(&jungle);
	newTerrain.biomeRegistry.push_back(&forest);
	newTerrain.biomeRegistry.push_back(&lake);
	newTerrain.biomeRegistry.push_back(&river);
	newTerrain.biomeRegistry.push_back(&ocean);
	newTerrain.biomeRegistry.push_back(&mountain);
	newTerrain.biomeRegistry.push_back(&tiaga);
	newTerrain.biomeRegistry.push_back(&tundra);
	newTerrain.biomeRegistry.push_back(&ice);
			
	newTerrain.regionRegistry.push_back(&hot);
	newTerrain.regionRegistry.push_back(&warm);	
	newTerrain.regionRegistry.push_back(&moderate);
	newTerrain.regionRegistry.push_back(&cool);
	newTerrain.regionRegistry.push_back(&cold);	
	}
	//while (!adv::ready);
	console::sleep(100);
	adv::setThreadState(false);
	int key = 0;
	fprintf(stderr, "Here\r\n");
	state.posX = 977.0f;
	state.posZ = 999.0f;
	state.scale = 16.90032f;
	state.actScale = 1.0f;
	state.heightMapView = 0;
	do {
		//fprintf(stderr, "::1\r\n");
		handleInput(key);
		
		adv::clear();
		
		//fprintf(stderr, "::2\r\n");
		for (int x = 0; x < adv::width; x++) {
			for (int z = 0; z < adv::height; z++) {
				wchar_t ch;
				color_t co;
				//newTerrain.generateClimate(x + state.posX, z + state.posZ, ch, co);
				//newTerrain.generate((x / state.scale) + state.posX, (z / state.scale) + state.posZ, ch, co);
				newNoiseFunction_t::haveFun((x / state.scale) + state.posX, (z / state.scale) + state.posZ, &ch, &co);
				adv::write(x,z,ch,co);				
				//adv::write(x,z,L'#',FWHITE|BBLACK);
			}
		}
		
		char buf[100];
		snprintf(&buf[0], 100, "[%f,%f] scale:%f camwidth:%f (%f minecraft blocks) view:%i", state.posX, state.posZ, state.scale, adv::width/state.scale, adv::width/state.scale * 100.0d, state.heightMapView);
		adv::write(0,0,&buf[0],FBLUE|BWHITE);
		//fprintf(stderr, "::3\r\n");
		adv::draw();
	} while (!HASKEY(key = console::readKey(), VK_ESCAPE));
	
	return 0;
}