/*
 * Background.cpp
 *
 *  Created on: 23 de abr. de 2016
 *      Author: gusta
 */

#include "Background.h"

Background::Background() {
	this->height = 0;
	this->scrollingSpeed = 0;
	this->width = 0;
	this->texture = NULL;
}

bool Background::loadBackground(string imagePath, SDL_Renderer* renderer){
	SDL_Surface* surfaceAux = IMG_Load(imagePath.c_str());
	if(surfaceAux == NULL){
		cout<<"Error al cargar la imagen del fondo"<<endl;//Deberiamos cargar una imagen con un "?"
		return false;
	}
	this->height = surfaceAux->h;
	this->width = surfaceAux->w;
	this->texture = SDL_CreateTextureFromSurface(renderer, surfaceAux);
	if(this->texture == NULL){
		cout<<"Error al crear la textura con la imagen del fondo"<<endl;
		return false;
	}
	SDL_FreeSurface(surfaceAux);
	return true;
}

bool Background::paint(SDL_Renderer* renderer, int posX, int posY){
	SDL_Rect backRect{posX, posY, width, height};
	if(SDL_RenderCopy(renderer,this->texture, NULL, &backRect) < 0)
		return false;
	return true;
}

int Background::getWidth(){
	return width;
}

Background::~Background() {
	SDL_DestroyTexture(texture);
}

