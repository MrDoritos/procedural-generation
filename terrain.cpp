#pragma once
#include <string>
#include <bits/stdc++.h>
#include "random.h"
#include "console.h"
#include "lodepng.h"
#include "png.h"
#include "pixel.h"
#include <vector>
#include <time.h>

class terrain {
	public:	
		int width, height;
		int size;
	
		char* cb;
		char* fb;		
		
		bool overlay;
		bool run;
		
		float walkMult;		
		static float posX, posY;
		static float multiplier;	
		float sensitivity;
		static float scale;
		//TO-DO Refactor this. No need to generate the noise all at once.
		float* map;
		int algorithm = 1;
		int algorithmCount = 2;
		
		
		float noiseSampler(float x, float y) {
			float noise = perlin::getPerlin(x, y);			
			
			//Linear transform -1:1 to 0:1
			return (noise + 1.0f)/(1.0f + 1.0f) * (1.0f);
		}
		
		//Generate a perlin noise map. Scale, for the camera.
		float* generateNoiseMap(float* noiseMap, int mapDepth, int mapWidth, float scale) {
			for (int zIndex = 0; zIndex < mapDepth; zIndex++) {
				for (int xIndex = 0; xIndex < mapWidth; xIndex++) {
					float sampleX = xIndex / scale;
					float sampleZ = zIndex / scale;					
					noiseMap[(zIndex * mapWidth) + xIndex] = noiseSampler(sampleX + posX, sampleZ + posY);
				}
			}			
			return noiseMap;
		}	
		
		//Take a normal and see if it is in a range of percents
		static bool range(float normal, float min, float max) {
			float perc = normal * 100.0f;
			return (perc >= min && perc <= max);
		}
		
		//Display the noise to the camera; print to screen.
		void showNoiseMap(float* map, int mapDepth, int mapWidth) {
			const auto generateValue = [](float* normal, char* color, char* character) {
				const char v[] = { " .:-=+%*@#" };
				float value = *normal;
				
				const auto getChar = [value,v](float min, float max) {
					float val = value * 100;
					float nom = (val / max);
					return v[int(floorf(nom * 10))];
				};
				
				//Terrain color, and characters to represent a change in elevation / distinguish changes in land color
				if(terrain::range(value, 0, 25)) {
					*color = BBLUE | FBLUE;
					*normal = 0.5f;
				} else
				if (terrain::range(value, 25, 50)) {
					*color = BBLUE | FWHITE;					
					*character = getChar(25, 50);
					*normal = 0.5f;
				} else
				if (terrain::range(value, 50, 90)) {
					*color = BYELLOW | FGREEN;
					*character = getChar(50, 90);
				} else 
				if (terrain::range(value, 90, 100)) {
					*color = BGREEN | FGREEN;
				}
				return;
			};
			
			float modifiedHeight;
			if (overlay)
				modifiedHeight = height / 2;
			else
				modifiedHeight = height;
			
			//Top down view
			for (int zIndex = 0; zIndex < modifiedHeight; zIndex++) {
				for (int xIndex = 0; xIndex < mapWidth; xIndex++) {
					int colorIndex = zIndex * mapWidth + xIndex;
					float normal = map[(zIndex * mapWidth) + xIndex];				
					char color = 0, character = 0;
					generateValue(&normal, &color, &character);
					cb[colorIndex] = color;
					fb[colorIndex] = character;
				}
			}
			
			//The cross section view
			if (overlay)
			for (int xIndex = 0; xIndex < mapWidth; xIndex++) {
				float normal = map[(1 * width) + xIndex];
				char color = 0, character = 0;
				float bad = normal;
				generateValue(&normal, &color, &character); //
				float nN = normal * (modifiedHeight);
				for (int y = 0; y < modifiedHeight; y++) {
					if (nN > y) {
						fb[(((height) - y - 1) * width) + xIndex] = character;
						cb[(((height) - y - 1) * width) + xIndex] = color;
					} else {
						cb[(((height) - y - 1) * width) + xIndex] = BBLACK | FBLACK;
					}
				}
			}			
			return;
		}
	
		terrain() {
			initialize();
		}
		
		terrain(int x, int y) {
			initialize();
			posX = x;
			posY = y;
		}
		
		void initialize() {
			updateConsoleSize();
			allocate();
			run = true;
			multiplier = 0.10f;			
			walkMult = 1.0f;
			scale = 6.40002f;
			posX = 41.0f;
			posY = 49.0f;
			perlin::persistence = 0.0f;
			sensitivity = 1.001f;
			perlin::octaves = 8.0f;						
			overlay = false;
			size = 5;
		}
		
		void handleInput() {
			switch (console::readKey()) {
					case 'q': 
					case 27:
					run = false;
					break;
					case '3':
					overlay = !overlay;
					break;
					case '7':
					multiplier += 0.01f;
					break;
					case '4':
					multiplier -= 0.01f;
					break;
					case '-':
					walkMult -= 5.0f;
					break;
					case '+':
					walkMult += 5.0f;
					break;
					case '8':
					perlin::octaves += 1.0f;
					break;
					case '5':
					perlin::octaves -= 1.0f;
					break;
					case '9':
					perlin::persistence += 0.025f;
					break;
					case '6':
					perlin::persistence -= 0.025f;
					break;
					case 'u':
					sensitivity += 0.1f;
					break;
					case 'j':
					sensitivity -= 0.1f;
					break;
					case '[':
					if (size > 1) size -= 1;
					break;
					case ']':
					if (size < 50) size += 1;
					break;
					case '.':
					if (algorithm == 1) {
						newScreenshot();
						break;
					}
					screenshot();
					break;
					case '1':
					scale -= 0.1f;
					break;
					case '2':
					scale += 0.1f;
					break;
					case 'w':
					posY -= walkMult / scale;
					break;
					case 's':
					posY += walkMult / scale;
					break;
					case 'a':
					posX -= walkMult / scale;
					break;
					case 'd':
					posX += walkMult / scale;
					break;
					case '/':
					algorithm = abs(--algorithm) % algorithmCount;
					break;
					case '*':
					algorithm = ++algorithm % algorithmCount;
					break;
					case 'f':
					px = --px % width;
					break;
					case 'h':
					px = ++px % width;
					break;
					case 't':
					py = --py % height;
					break;
					case 'g':
					py = ++py % height;
					break;
			}
		}			
		
		#ifdef screenshot
		
		void screenshot() {			
			//This is a shitty way of creating a PNG. Fix? Nah. (Too much memory goes to waste, luckily no leaks :) )
			std::vector<unsigned char> rawImage;
			rawImage.resize((width * size) * (height * size) * 4);
	
			png image(rawImage, width * size, height * size);
			int sizeX = width * size;
			int sizeY = height * size;
			
			
			const auto generateValue = [](float normal, pixel* pixel) {
				const char v[] = { " .:-=+%*#@" };
				float value = normal;
				
				const auto convertRange = [value,v](float min, float max) {
					float val = value * 100;
					float nom = ((val - min) / max);
					return nom * 255;
				};
				
				if(terrain::range(value, 0, 25)) {
					pixel->blue = 255;
					pixel->red = 0;
					pixel->green = 0;
					pixel->alpha = 255;
					normal = 0.5f;
				} else
				if (terrain::range(value, 25, 50)) {
					pixel->blue = 255;
					pixel->red = convertRange(25, 50);
					pixel->green = convertRange(25,50);
					pixel->alpha = 255;
				} else
				if (terrain::range(value, 50, 90)) {
					pixel->blue = 0;
					pixel->red = convertRange(50, 90) - 255;
					pixel->green = convertRange(50, 90);
					pixel->alpha = 255;
				} else 
				if (terrain::range(value, 90, 100)) {
					pixel->blue = 0;
					pixel->red = 0;
					pixel->green = convertRange(90, 100);
					pixel->alpha = 255;
				}				
			};
			
			float newScaleX = image.getSizeX() / (width / scale);
			float newScaleY = image.getSizeY() / (height / scale);
			
			for (int x = 0; x < image.getSizeX(); x++) {
				for (int y = 0; y < image.getSizeY(); y++) {
					pixel* p = image.getPixel(x, y);
					generateValue(noiseSampler(posX + x / newScaleX, posY + y / newScaleY), p);
				}
				int val = int((float(x) / float(image.getSizeX())) * 100.0f);
				std::string pro = std::to_string(val);
				std::string progress = pro + "% complete ";
				console::write(0,0,progress);
			}
			
			//This is why it's shitty
			image.deconvert(rawImage);
			
			time_t timer;
			time(&timer);
			std::string integer = std::to_string(timer);
			std::string fin = integer + ".png";
			
			unsigned error = lodepng::encode(fin.c_str(), rawImage, width * size, height * size);
	
			if (error) {
				fprintf(stderr, "encoder error %i : %s\r\n", error, lodepng_error_text(error));
			}				
		}
			
		void newScreenshot() {
			std::vector<unsigned char> rawImage;
			rawImage.resize((width * size) * (height * size) * 4);
	
			png image(rawImage, width * size, height * size);
			int sizeX = width * size;
			int sizeY = height * size;
			
			
			float newScaleX = image.getSizeX() / (width / scale);
			float newScaleY = image.getSizeY() / (height / scale);
			
			for (int x = 0; x < image.getSizeX(); x++) {
				for (int y = 0; y < image.getSizeY(); y++) {
					pixel* p = image.getPixel(x, y);
					//generateValue(noiseSampler(posX + x / newScaleX, posY + y / newScaleY), p);
					double noise = getNormalNoise(posX + x / newScaleX, posY + y / newScaleY);
					mapv(noise, p);
				}
				int val = int((float(x) / float(image.getSizeX())) * 100.0f);
				std::string pro = std::to_string(val);
				std::string progress = pro + "% complete ";
				console::write(0,0,progress);
			}
			
			image.deconvert(rawImage);
			
			time_t timer;
			time(&timer);
			std::string integer = std::to_string(timer);
			std::string fin = integer + ".png";
			
			unsigned error = lodepng::encode(fin.c_str(), rawImage, width * size, height * size);
	
			if (error) {
				fprintf(stderr, "encoder error %i : %s\r\n", error, lodepng_error_text(error));
			}			
		}
			
		#else
			
		void screenshot() {}
		
		void newScreenshot() {}
		
		void mapv(double value, pixel* p) {
			char ch, color;
			
			p->red = value * 255;
			p->alpha = 255;
		}
		
		#endif
		
			
		void mapValue(double value, char& ch, char& color) {
			const char  clvls[] = " -+##+- ";
			const char colors[] = { FBLUE, FWHITE, FYELLOW, FGREEN, FGREEN };
			
			const int groups = sizeof(colors) - 1;
			
			int group = int(floor(value * groups));
			char color0 = colors[group];
			char color1 = colors[group + 1];
			
			double normalizedGroupValue = value / ((group + 1.0d) / groups);
			
			if (normalizedGroupValue >= 0.5d) {
				color = color0 | (color1 << 4);
			} else {
				color = color1 | (color0 << 4);
			}
			
			ch = clvls[int(normalizedGroupValue * (sizeof(clvls) - 1))];			
		}
		
		static double getNormal(double omax, double omin, double max, double min, double value) {
			return (max - min)/(omax-omin)*(value-omax)+max;
		}
		
		static double getMasterNoise(double x, double z) {
			double sampleX = x / scale;
			double sampleZ = z / scale;
			return getNormalNoise(sampleX + posX, sampleZ + posY);
		}
		
		static double getNormalNoise(double x, double z) {
			double noise = perlin::getPerlin(x, z);
			
			return getNormal(1,-1,1+multiplier,0-multiplier,noise);
		}
			
		void showNewAlgorithm() {
			double modifiedHeight;
			if (overlay)
				modifiedHeight = height / 2;
			else
				modifiedHeight = height;
			
			struct biome {
				biome(double weight):weight(weight) {}
				virtual double getValueAt(double x, double z) = 0;			
				double weight;
			};
			
			struct mountain : public biome {
				mountain():biome(0.4f) {}
				double getValueAt(double x, double z) override {
					double noise = getMasterNoise(x,z) * weight;
					noise *= 128;
					noise += 72;
				}
			} const b_mountain;
			
			struct beach : public biome {
				beach():biome(0.2f) {}
				double getValueAt(double x, double z) override {
					double noise = getMasterNoise(x,z) * weight;
					noise *= 16; //Give beaches a range from 0 to 16
					noise += 64; //Starting at 64
				}
			} const b_beach;
			
			struct ocean : public biome {
				ocean():biome(0.4f) {}
				double getValueAt(double x, double z) override {
					double noise = getMasterNoise(x,z) * weight;
					noise *= 128; //Give ocean a range from 0 - 128
					noise += 0; //Starting at 0
				}
			} const b_ocean;
			
			const biome* biomes[] = { (const biome*)&b_mountain, &b_beach, &b_ocean };
			double n = sizeof(biomes) / sizeof(biomes[0]);
			
			for (double x = 0; x < width; x++) {
				for (double z = 0; z < modifiedHeight; z++) {
					double l = getMasterNoise(x,z);
					double y = 0;
					for (int i = 0; i < sizeof(biomes) / sizeof(biomes[0]); i++) {
						if ((i - 1.0f) / n <= l && l <= (i + 1.0f) / n)
							y += (-abs(n * l - i) + 1) * biomes[i]->getValueAt((x / scale) + posX, (z / scale) + posY);
					}
					
					char ch, c;
					mapValue(y / sensitivity, ch, c);
					fb[int((z * width) + x)] = ch;
					cb[int((z * width) + x)] = c;
				}
			}
			
			
			if (overlay)
			for (int xIndex = 0; xIndex < width; xIndex++) {
				double normal = getMasterNoise(xIndex, 1);
				char color = 0, character = 0;
				float bad = normal;
				mapValue(normal, character, color);
				double nN = normal * (modifiedHeight);
				for (int y = 0; y < modifiedHeight; y++) {
					if (nN > y) {
						fb[(((height) - y - 1) * width) + xIndex] = character;
						cb[(((height) - y - 1) * width) + xIndex] = color;
					} else {
						cb[(((height) - y - 1) * width) + xIndex] = BBLACK | FBLACK;
					}
				}
			}	
		}			
			
		int px, py;
			
		void loop() {			
			char ll[100];
			while (run) {				
				double noise = 0.0d;
				if (algorithm == 0) {
					showNoiseMap(generateNoiseMap(map, height, width, scale), height, width);	
					noise = 0;
				}
				if (algorithm == 1) {
					showNewAlgorithm();
					noise = getMasterNoise(px,py);
				}
				sprintf(ll, "Oct %.2f Per %.2f Scl %.2f Mult %.2f [%.2f %.2f] (%.2fx) [%ix] %i w:%i h:%i v:%.2f s:%.2f", perlin::octaves, perlin::persistence, scale, multiplier, posX, posY, walkMult, size, algorithm, width, height, noise, sensitivity);
				for (int i = 0; i < 100 && i < width; i++) {
					if (ll[i] == '\0')
						break;
					fb[i] = ll[i];
					cb[i] = FWHITE | 00001000 | BBLACK;
				}		
				write();
				console::setCursorPosition(px,py);
				handleInput();
				console::setCursorPosition(px,py);
			}
		}
		
		void updateConsoleSize() {
			width = console::getConsoleWidth();
			height = console::getConsoleHeight();
		}
	
		void allocate() {
			cb = new char[width * height];
			fb = new char[width * height];
			map = new float[width * height];
			clear();
		}
	
		void clear() {
			clearBuffer();
			write();
		}
	
		void clearBuffer() {
			memset(fb, ' ', width * height);
			memset(cb, FWHITE | BBLACK, width * height);			
		}
	
		void write() {
			console::write(fb, cb, width * height);
		}		
};

float terrain::posX;
float terrain::posY;
float terrain::scale;
float terrain::multiplier;

int main() {
	terrain terra;
	terra.loop();	
	return 0;
}