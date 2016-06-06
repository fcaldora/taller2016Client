/*
 * Score.cpp
 *
 *  Created on: 31 de may. de 2016
 *      Author: gusta
 */

#include "Score.h"

Score::Score() {
	pointsTexture = NULL;
	points = 0;
	nameTexture = NULL;
	fontType = NULL;
	renderer = NULL;
	posX = 0;
	posY = 0;
	fontSize = 0;
	clientName = "";
	SDL_StartTextInput();
	TTF_Init();
}

int Score::getPoints(){
	return points;
}

void Score::setName(string name){
	clientName = name;
	if(nameTexture != NULL)
		SDL_DestroyTexture(nameTexture);
	SDL_Color colorText;
	colorText.b = 255;
	colorText.g = 255;
	colorText.r = 255;
	SDL_Surface* nameSurface = TTF_RenderText_Solid(this->fontType, clientName.c_str(), colorText);
	if(nameSurface == NULL){
		cout<<"Error al cargar la surface del nombre en el puntaje"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
	if(nameTexture == NULL){
		cout<<"Error al cargar la textura del nombre en el puntaje"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(nameSurface);
}

void Score::paint(){
	SDL_Rect pointsRectangle;
	pointsRectangle.h = fontSize*2;
	pointsRectangle.w = fontSize*5;
	pointsRectangle.x = posX;
	pointsRectangle.y = posY;
	SDL_Rect nameRectangle;
	nameRectangle.h = fontSize*2;
	nameRectangle.w = 100;
	nameRectangle.x = posX;
	nameRectangle.y = posY - 3*fontSize; //dibujo el nombre sobre los puntos.

	if(pointsTexture != NULL){
		SDL_RenderCopy(renderer, pointsTexture, NULL, &pointsRectangle);
	}
	if(nameTexture != NULL){
		SDL_RenderCopy(renderer, nameTexture, NULL, &nameRectangle);
	}
}

void Score::setFontType(string fontPath, int fontSize){
	fontType =  TTF_OpenFont( fontPath.c_str(), fontSize );
	if(fontType == NULL)
		cout<<"Error al cargar la fuente para el puntaje"<<endl;
	this->fontSize = fontSize;
}

void Score::setRenderer(SDL_Renderer* windowRenderer){
	this->renderer = windowRenderer;
}

void Score::setPoints(int points){
	this->points = points;
	if(pointsTexture != NULL)
		SDL_DestroyTexture(pointsTexture);
	SDL_Color colorText;
	colorText.b = 255;
	colorText.g = 255;
	colorText.r = 255;
	SDL_Surface* scoreSurface = TTF_RenderText_Solid( this->fontType, std::to_string(points).c_str(), colorText );
	if(scoreSurface == NULL){
		cout<<"Error al cargar la surface del puntaje"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	pointsTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
	if(pointsTexture == NULL){
		cout<<"Error al cargar la textura del puntaje"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(scoreSurface);
}

void Score::setPosition(int posx, int posy){
	this->posX = posx;
	this->posY = posy;
}

Score::~Score() {
	if(pointsTexture != NULL)
		SDL_DestroyTexture(pointsTexture);
	if(nameTexture != NULL)
		SDL_DestroyTexture(nameTexture);
	SDL_StopTextInput();
	TTF_Quit();
}

