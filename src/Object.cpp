/*
 * Object.cpp
 *
 *  Created on: Apr 25, 2016
 *      Author: facundo
 */

#include "Object.h"

Object::Object() {
	// TODO Auto-generated constructor stub

}

Object::~Object() {
	// TODO Auto-generated destructor stub
}

bool Object::loadImage(string pathImage, SDL_Renderer* renderer, int width, int height){
	SDL_Surface* surfaceAux = IMG_Load(pathImage.c_str());
	if(surfaceAux == NULL){
		cout<<"Error al cargar la imagen del avion"<<endl;
		surfaceAux = IMG_Load(imageErrorPath);
	}
	this->height = height;
	this->width = width;
	//color key = (0,255,255)
	SDL_SetColorKey( surfaceAux, SDL_TRUE, SDL_MapRGB( surfaceAux->format, 0, 255, 255 ) );
	this->texture = SDL_CreateTextureFromSurface(renderer, surfaceAux);
	if(this->texture == NULL){
		cout<<"Error al crear la textura con la imagen del avion"<<endl;
		return false;
	}
	SDL_FreeSurface(surfaceAux);
	return true;
}

bool Object::paint(SDL_Renderer* renderer, int posX, int posY){
	SDL_Rect imageRect{posX,posY,this->width, this->height};
	int imageX = width * (this->actualPhotogram - 1);
	SDL_Rect photogramRect{imageX, 0, width, height};
	if(SDL_RenderCopy(renderer,this->texture, &photogramRect, &imageRect) < 0){
		return false;
	}
	return true;
}
