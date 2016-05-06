/*
 * Avion.cpp
 *
 *  Created on: 22 de abr. de 2016
 *      Author: gusta
 */

#include "Avion.h"

Avion::Avion(string name, int posX, int posY) {
	this->name = name;
	this->posX = posX;
	this->posY = posY;
	this->texture = NULL;
	this->height = 0;
	this->width = 0;
}

bool Avion::loadImage(string pathImage, SDL_Renderer* renderer, int width, int heigth){
	SDL_Surface* surfaceAux = IMG_Load(pathImage.c_str());
	if(surfaceAux == NULL){
		cout<<"Error al cargar la imagen del avion"<<endl;//Deberiamos cargar una imagen con un "?"
		surfaceAux = IMG_Load(imageErrorPath);
	}
	this->height = height;
	this->width = width;
	//Pongo como color key el cyan (0,255,255) para que se pinte solo el avion.
	SDL_SetColorKey( surfaceAux, SDL_TRUE, SDL_MapRGB( surfaceAux->format, 0, 255, 255 ) );
	this->texture = SDL_CreateTextureFromSurface(renderer, surfaceAux);
	if(this->texture == NULL){
		cout<<"Error al crear la textura con la imagen del avion"<<endl;
		return false;
	}
	SDL_FreeSurface(surfaceAux);
	return true;
}

bool Avion::paint(SDL_Renderer* renderer, int posX, int posY){
	SDL_Rect imageRect{posX,posY,this->width, this->height};
	if(SDL_RenderCopy(renderer,this->texture, NULL, &imageRect) < 0){
		return false;
	}
	return true;
}

//Aca se envian los mensajes al Server
int Avion::processEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN){
		int value = 0;
		switch(event->key.keysym.sym){
			case SDLK_DOWN:
				value = 1;
				break;
			case SDLK_UP:
				value = 2;
				break;
			case SDLK_RIGHT:
				value = 3;
				break;
			case SDLK_LEFT:
				value = 4;
				break;
			case SDLK_SPACE:
				value = 5;
				break;
			case SDLK_r:
				value = 6;
				break;
			case SDLK_a:
				value = 7;
				break;
			}
		return value;

	} else if(event->type == SDL_QUIT){
		return -1;
	}
	return 0;

}

Avion::~Avion() {
	SDL_DestroyTexture(texture);
}

