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

bool Avion::loadImage(string pathImage, SDL_Renderer* renderer){
	SDL_Surface* surfaceAux = IMG_Load(pathImage.c_str());
	if(surfaceAux == NULL){
		cout<<"Error al cargar la imagen del avion"<<endl;//Deberiamos cargar una imagen con un "?"
		return false;
	}
	this->height = surfaceAux->h;
	this->width = surfaceAux->w;
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
bool Avion::processEvent(SDL_Event* event){
	if(event->type == SDL_KEYDOWN){
		switch(event->key.keysym.sym){
		case SDLK_DOWN:
			cout<<"ABAJO"<<endl;
			break;
		case SDLK_UP:
			cout<<"ARRIBA"<<endl;
			break;
		case SDLK_RIGHT:
			cout<<"DERECHA"<<endl;
			break;
		case SDLK_LEFT:
			cout<<"IZQUIERDA"<<endl;
			break;
		case SDLK_SPACE:
			cout<<"DISPARAR"<<endl;
			break;
		}
	}else if(event->type == SDL_QUIT){
		return true;
	}
	return false;
}

Avion::~Avion() {
	SDL_DestroyTexture(texture);
}

