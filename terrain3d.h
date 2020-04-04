#pragma once
#include <string>
#include <bits/stdc++.h>
#include "random.h"
#include "console.h"

#include <iostream>
#include <fstream>

//She will manage the terrain generation. This class will not worry about generation.
#include "generator3d.h"

//No allocation tables?

class terrain3d {
	public:
		double posX, posY, posZ;
		double scale;
		bool run, top, side;
		char *fb, *cb;
		int width, height;
		int wLength;
		
		terrain3d() {
			initialize();
		}		
		
		void initialize() {			
			updateConsoleSize();
			allocate(wLength);
			posX = 0;
			posY = 0;
			posZ = 0;
			run = true;
			perlin::octaves = 8;
			perlin::persistence = 0.5;
			scale = 2.5d;
			top = true;
			side = true;
		}
		
		void allocate(int length) {
			fb = new char[length];
			cb = new char[length];
		}
		
		void updateConsoleSize() {
			width = console::getConsoleWidth();
			height = console::getConsoleHeight();
			wLength = width * height;
		}
		
		//All we need to do is provide an implementation
		/*
		void generateBlockAt(int x, int y, int z, char &character, char &color) {
			character = 'A';
			color = FRED | BWHITE;
		}
		*/
		
		void write(int x, int y, char character, char color = FWHITE | BBLACK) {
			int offset = (y * width) + x;
			fb[offset] = character;
			cb[offset] = color;
		}
		
		void write() {
			console::write(fb, cb, wLength);
		}
		
		void clearBuffer() {
			memset(fb, ' ', width * height);
			memset(cb, FWHITE | BBLACK, width * height);			
		}
		
		void handleKeyboard() {
			switch (console::readKey()) {
				case 'q':
					run = false;
				break;
				case '8':
					perlin::octaves += 1.0f;
				break;
				case '5':
					perlin::octaves -= 1.0f;
				break;
				case '9':
					perlin::persistence += 0.05f;
				break;
				case '6':
					perlin::persistence -= 0.05f;
				break;
				case '1':
					scale -= 0.1d;
				break;
				case '2':
					scale += 0.1d;
				break;
				case 'w':
					posY -= 5.0d / scale;
				break;
				case 's':
					posY += 5.0d / scale;
				break;
				case 'a':
					posX -= 5.0d / scale;
				break;
				case 'd':
					posX += 5.0d / scale;
				break;
				case 'x':
					posZ += 5.0d / scale;
				break;
				case 'z':
					posZ -= 5.0d / scale;
				break;
				case '3':
					side = !side;
				break;
				case '.':
					top = !top;
				break;
				default: break;
			}
		}
		
		void loop() {
			std::string working = "working...";
			while (run) {
				
				console::write(0,0,working);
				//Split the console in 2 halves, render
				double half = floor(height / 2.0d);
				char character, color;
				//Top view
				if (top)
				for (int y = 0; y < half; y++) {
					for (int x = 0; x < width; x++) {						
						generator3d::generateBlockAt(x / scale + posX, y / scale + posY, posZ, character, color);
						write(x, y, character, color);
					}
				}
				//Side view //offset == half
				if (side)
				for (int z = 0; z < half; z++) {
					for (int x = 0; x < width; x++) {
						generator3d::generateBlockAt(x / scale + posX, posY, z / scale + posZ, character, color);
						write(x, z + half, character, color);
					}
				}
				
				write();
				clearBuffer();
				handleKeyboard();
			}
		}
};