#pragma once
#include "random.h"
#include "console.h"
#include <cmath>
#include <string.h>

class starSystem {
	public:
		starSystem(uint32_t x, uint32_t y) {
			random::procGen = (x & 0xFFFF) << 16 | (y & 0xFFFF);
			starExists = (random::getInt(0, 20) == 1);
			if (!starExists) return;
			
			starDiameter = random::getDouble(10.0d, 40.0d);
			starColor = char(random::getInt(0, 8));
		}
		
		bool starExists = false;
		double starDiameter = 0.0f;
		char starColor = FWHITE;
};

struct vi2d {
	uint32_t x, y;
};
	
class program {
	public:
	
	int sectorsX, sectorsY;
		
	double posX, posY;
	
	int width, height;
	
	char* cb;
	char* fb;
	
	double mapScale = 0.08d;
	
	bool run;
		
	program() {
		posX = 0; posY = 0;		
		updateConsoleSize();
		sectorsX = width / (width * mapScale);
		sectorsY = height / (width * mapScale);		
		run = true;
		allocate();
	}
	
	program(int x, int y) {
		posX = x; posY = y;
		updateConsoleSize();
		sectorsX = width / (width * mapScale);
		sectorsY = height / (width * mapScale);		
		run = true;
		allocate();
	}
	
	void updateConsoleSize() {
		width = console::getConsoleWidth();
		height = console::getConsoleHeight();
	}
	
	void allocate() {
		cb = new char[width * height];
		fb = new char[width * height];
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
		
	void handleKey(int key) {
		const double move = 1.0d;
		switch ((char)key) {
			case 'w':
				posY -= move;
				break;
			case 's':
				posY += move;
				break;
			case 'a':
				posX -= move;
				break;
			case 'd':
				posX += move;
				break;
			case 'q':
				run = false;
				break;
		}		
		fprintf(stderr, "%c [%hf,%hf]\r\n", char(key), posX, posY);
	}
	
	void loop() {
		while (run) {
			clearBuffer();
			
			handleKey(console::readKey());
			
			vi2d screen_sector = { 0,0 };
			
			for (screen_sector.x = 0; screen_sector.x < sectorsX; screen_sector.x++) {
				for (screen_sector.y = 0; screen_sector.y < sectorsY; screen_sector.y++) {
					starSystem star(screen_sector.x + posX, screen_sector.y + posY);
					
					if (star.starExists) {
						drawCircle(screen_sector.x * (width * mapScale) + (width * mapScale / 2), screen_sector.y * (height * mapScale) + (width * mapScale / 2), (int)star.starDiameter / (width * mapScale / 2), star.starColor);
					}
				}
			}
				
			
			write();
			//console::sleep(40);
		}
	}
	
	void drawCircle(double x, double y, double radius, char color) {
		double step = 0.05;//1 / (radius * D_PI);
		int xF, yF;
		for (double theta = 0.0d; theta < DOUBLE_PI; theta += step) {
			xF = (x + radius * cos(theta));
			yF = (y - radius * sin(theta));
			if ((xF >= 0 && yF >= 0) && (xF < width && yF < height)) {
				fb[(yF * width) + xF] = '#';
				cb[(yF * width) + xF] = color;
			}
		}
	}
};
