/*
 * Avion.h
 *
 *  Created on: 22 de abr. de 2016
 *      Author: gusta
 */
#include <string>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

#ifndef AVION_H_
#define AVION_H_

using namespace std;

class Avion {
public:
	Avion(string name,int posX, int posY);
	bool loadImage(string imagePath, SDL_Renderer* renderer, int width, int heigth);
	bool paint(SDL_Renderer* renderer, int posX, int posY);
	int processEvent(SDL_Event* event);
	virtual ~Avion();

private:
	string name;
	string imagePath;
	SDL_Texture* texture;
	int posX, posY;
	int height, width;
};

#endif /* AVION_H_ */
