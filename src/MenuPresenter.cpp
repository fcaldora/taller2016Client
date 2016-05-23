/*
 * MenuPresenter.cpp
 *
 *  Created on: 19 de may. de 2016
 *      Author: gusta
 */

#include "MenuPresenter.h"

MenuPresenter::MenuPresenter() {
	this->inputTexture = NULL;
	this->backgroundTexture = NULL;
	this->renderer = NULL;
	this->textFont = NULL;
	this->resultMsg = NULL;
	this->playerName = "";
	this->fontSize = 0;
	this->width = 0;
	this->height = 0;
}

SDL_Texture* MenuPresenter::getResultTexture(){
	return this->resultMsg;
}

bool MenuPresenter::presentNameMenu(){
	bool end = false;
	SDL_StartTextInput();
	TTF_Init();
	string textFontPath = "Caviar_Dreams_Bold.ttf";
	this->fontSize = 25;
	this->textFont = TTF_OpenFont( textFontPath.c_str(), fontSize );
	if(this->textFont == NULL){
		cout<<"Error al cargar la fuente del texto"<<endl;
		cout<<TTF_GetError()<<endl;
		return false;
	}
	while (!end){
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			paint();
			if(event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
				case SDLK_KP_ENTER:
					end = true;
					SDL_StopTextInput();
					return true;
					break;
				case SDLK_BACKSPACE:
					if( this->playerName.size() > 0){
						this->playerName.pop_back();
						setInputTexture();
					}
					break;
				}
			}
			switch(event.type){
			case SDL_QUIT:
				end = true;
				SDL_StopTextInput();
				return false;
				break;
			case SDL_TEXTINPUT:
				this->playerName.append(event.text.text);
				setInputTexture();
				break;
			}
		}
	}
	cout<<"Player name: "<<playerName<<endl;
	SDL_StopTextInput();
	return true;
}

void MenuPresenter::loadMenuBackground(string pathImage, int height, int width){
	SDL_Surface* surface = IMG_Load(pathImage.c_str());
	if(surface == NULL){
		cout<<"Error al cargar el fondo del menú"<<endl;
		return;
	}
	this->height = height;
	this->width = width;

	this->backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
	if(this->backgroundTexture == NULL){
		cout<<"Error al crear la textura del fondo del menu"<<endl;
		SDL_FreeSurface(surface);
		return;
	}

	paint();
	SDL_FreeSurface(surface);
}

void MenuPresenter::paint(){
	int rectPosX = width/2 - this->playerName.size()*5;
	SDL_Rect inputRect{rectPosX, height/2 - 30, this->playerName.size()*fontSize, 2*fontSize};
	SDL_Rect textRect{width/2, height/2 + 30,200, 2*fontSize};
	if(this->backgroundTexture != NULL)
		SDL_RenderCopy(renderer, this->backgroundTexture, NULL, NULL);
	if(this->inputTexture != NULL)
		SDL_RenderCopy(renderer, this->inputTexture, NULL, &inputRect);
	if(this->resultMsg != NULL){
		SDL_RenderCopy(renderer, this->resultMsg, NULL, &textRect);
	}
	SDL_RenderPresent(renderer);
}

string MenuPresenter::getPlayerName(){
	return this->playerName;
}

void MenuPresenter::setRenderer(SDL_Renderer* renderer){
	this->renderer = renderer;
}

void MenuPresenter::destroyTexture(SDL_Texture* texture){
	if(texture != NULL){
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
}

void MenuPresenter::erasePlayerName(){
	this->playerName = "";
}

void MenuPresenter::setInputTexture(){
	destroyTexture(this->inputTexture);
	SDL_Color colorText;
	colorText.b = 255;
	colorText.g = 255;
	colorText.r = 255;
	SDL_Surface* textSurface = TTF_RenderText_Solid( this->textFont, playerName.c_str(), colorText );
	if(textSurface == NULL && playerName.size() > 0){
		cout<<"Error al cargar la surface del texto"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	inputTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if( inputTexture == NULL && playerName.size() > 0){
		cout<<"Error al cargar la textura del texto"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(textSurface);
}

void MenuPresenter::setResultTexture(string result){
	destroyTexture(this->resultMsg);
	SDL_Color colorText;
	colorText.b = 255;
	colorText.g = 255;
	colorText.r = 255;
	SDL_Surface* textSurface = TTF_RenderText_Solid( this->textFont, result.c_str(), colorText );
	if(textSurface == NULL && result.size() > 0){
		cout<<"Error al cargar la surface del texto"<<endl;
		cout<<TTF_GetError()<<endl;
		return;
	}
	resultMsg = SDL_CreateTextureFromSurface(renderer, textSurface);
	if( resultMsg == NULL && result.size() > 0){
		cout<<"Error al cargar la textura del texto"<<endl;
		cout<<SDL_GetError()<<endl;
	}
	SDL_FreeSurface(textSurface);
}


MenuPresenter::~MenuPresenter() {
	if(this->backgroundTexture != NULL)
		SDL_DestroyTexture(this->backgroundTexture);
	if(this->inputTexture != NULL)
		SDL_DestroyTexture(this->inputTexture);
	if(this->resultMsg != NULL)
		SDL_DestroyTexture(this->resultMsg);
	SDL_StopTextInput();
	TTF_Quit();
}

