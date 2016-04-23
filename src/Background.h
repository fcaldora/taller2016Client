/*
 * Background.h
 *
 *  Created on: 23 de abr. de 2016
 *      Author: gusta
 */
#include <string>
#include <SDL2/SDL_image.h>
#include <iostream>
#ifndef BACKGROUND_H_
#define BACKGROUND_H_

using namespace std;

class Background {
public:
	Background();
	bool loadBackground(string imagePath, SDL_Renderer* renderer);
	bool paint(SDL_Renderer* renderer, int posY, int posX);
	int getWidth();
	virtual ~Background();

private:
	int height, width;
	string imagePath;
	int scrollingSpeed;
	SDL_Texture* texture;
};

#endif /* BACKGROUND_H_ */
