/*
 * StageInfo.cpp
 *
 *  Created on: 3 de jun. de 2016
 *      Author: gusta
 */

#include "StageInfo.h"

StageInfo::StageInfo() {
	fontType = NULL;
	stageInfoTexture = NULL;
	pointsInfoTexture = NULL;
	renderer = NULL;
	hasToPaint = false;
	fontSize = 0;
	posX = 0;
	posY = 0;
	paintCounter = 0;
	pointsInfo = "Puntos: ";
	stageInfo = "Fin de etapa ";
}

void StageInfo::setHasToPaint(bool hasTo){
	hasToPaint = hasTo;
}

void StageInfo::setPointsInfo(int  points){
	string pInfo = pointsInfo + std::to_string(points);
	if(pointsInfoTexture != NULL)
		SDL_DestroyTexture(pointsInfoTexture);
	SDL_Color colorText;
	colorText.b = 0;
	colorText.g = 0;
	colorText.r = 255;
	SDL_Surface* infoSurface = TTF_RenderText_Solid( this->fontType, pInfo.c_str(), colorText );
	if(infoSurface == NULL){
		cout<<"Error al cargar la surface de la info puntaje"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	pointsInfoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
	if(pointsInfoTexture == NULL){
		cout<<"Error al cargar la textura de la info puntaje"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(infoSurface);
}

void StageInfo::setPositions(int posx, int posy){
	posX = posx;
	posY = posy;
}

void StageInfo::setRenderer(SDL_Renderer* windowRenderer){
	renderer = windowRenderer;
}

void StageInfo::setStageInfo(int numberOfStage){
	string sInfo = stageInfo + std::to_string(numberOfStage);
	if(stageInfoTexture != NULL)
		SDL_DestroyTexture(stageInfoTexture);
	SDL_Color colorText;
	colorText.b = 0;
	colorText.g = 0;
	colorText.r = 0;
	SDL_Surface* infoSurface = TTF_RenderText_Solid( this->fontType, sInfo.c_str(), colorText );
	if(infoSurface == NULL){
		cout<<"Error al cargar la surface del stage info"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	stageInfoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
	if(stageInfoTexture == NULL){
		cout<<"Error al cargar la textura del stage info"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(infoSurface);
}

void StageInfo::setFontType(string path, int fontSize){
	fontType =  TTF_OpenFont( path.c_str(), fontSize );
	if(fontType == NULL)
		cout<<"Error al cargar la fuente para el puntaje"<<endl;
	this->fontSize = fontSize;
}

bool StageInfo::paintNow(){
	if(hasToPaint)
		paintCounter++;
	if(paintCounter == 0 || paintCounter == TIMEPAINT){
		hasToPaint = false;
		paintCounter = 0;
		return false;
	}
	return true;
}

void StageInfo::paint(){
	SDL_Rect stageRect;
	stageRect.x = posX;
	stageRect.y = posY;
	stageRect.h = 3*fontSize;
	stageRect.w = stageInfo.size() * fontSize;
	SDL_Rect pointsRect;
	pointsRect.x = posX;
	pointsRect.y = posY + stageRect.h;
	pointsRect.h = stageRect.h;
	pointsRect.w = pointsInfo.size() * fontSize;

	if(stageInfoTexture != NULL){
		SDL_RenderCopy(renderer, stageInfoTexture, NULL, &stageRect);
	}
	if(pointsInfoTexture != NULL){
		SDL_RenderCopy(renderer, pointsInfoTexture, NULL, &pointsRect);
	}
}

void StageInfo::setEndGameInfo(){
	if(stageInfoTexture != NULL)
		SDL_DestroyTexture(stageInfoTexture);
	SDL_Color colorText;
	colorText.b = 0;
	colorText.g = 0;
	colorText.r = 0;
	SDL_Surface* infoSurface = TTF_RenderText_Solid( this->fontType, "Fin del juego", colorText );
	if(infoSurface == NULL){
		cout<<"Error al cargar la surface del stage info"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	stageInfoTexture = SDL_CreateTextureFromSurface(renderer, infoSurface);
	if(stageInfoTexture == NULL){
		cout<<"Error al cargar la textura del stage info"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(infoSurface);
}

StageInfo::~StageInfo() {
}

