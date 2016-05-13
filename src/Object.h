/*
 * Object.h
 *
 *  Created on: Apr 24, 2016
 *      Author: facundo
 */

#ifndef OBJECT_H_
#define OBJECT_H_
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "Constants.h"

using namespace std;

class Object {
public:
	Object();
	virtual ~Object();

	int getHeight() const {
		return height;
	}

	void setHeight(int height) {
		this->height = height;
	}

	int getId() const {
		return id;
	}

	void setId(int id) {
		this->id = id;
	}

	const string& getPath() const {
		return imagePath;
	}

	void setPath(const string& path) {
		this->imagePath = path;
	}

	int getPhotograms() const {
		return photograms;
	}

	void setPhotograms(int photograms) {
		this->photograms = photograms;
	}

	int getPosX() const {
		return posX;
	}

	void setPosX(int posX) {
		this->posX = posX;
	}

	int getPosY() const {
		return posY;
	}

	void setPosY(int posY) {
		this->posY = posY;
	}

	int getWidth() const {
		return width;
	}

	void setWidth(int width) {
		this->width = width;
	}

	void setActualPhotogram(int photogram){
		this->actualPhotogram = photogram;
	}

	void destroyTexture();

	bool paint(SDL_Renderer* renderer, int posX, int posY);

	bool loadImage(string pathImage, SDL_Renderer* renderer, int width, int heigth);

private:
	int id;
	string imagePath;
	int actualPhotogram;
	int photograms;
	int posX;
	int posY;
	int width;
	int height;
	SDL_Texture* texture;
};

#endif /* OBJECT_H_ */
