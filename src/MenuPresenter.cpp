/*
 * MenuPresenter.cpp
 *
 *  Created on: 19 de may. de 2016
 *      Author: gusta
 */

#include "MenuPresenter.h"

#include <string>

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

string MenuPresenter::presentCreateTeamOptionAndGetName() {
	string userDidEnterName;
	SDL_StartTextInput();

	while (true){
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
				case SDLK_RETURN:
					SDL_StopTextInput();
					return userDidEnterName;
					break;
				case SDLK_BACKSPACE:
					if( userDidEnterName.size() > 0){
						userDidEnterName.pop_back();
						this->addinputTextAtLine(userDidEnterName, 6);
					}
					break;
				}
			}
			switch(event.type){
			case SDL_QUIT:
				SDL_StopTextInput();
				return "";
				break;
			case SDL_TEXTINPUT:
				userDidEnterName.append(event.text.text);
				this->addinputTextAtLine(userDidEnterName, 6);
				break;
			}
		}
	}
}

string MenuPresenter::presentCreateOrJoinTeamOptionMenuAndGetSelectedOption(vector <string> posibleOptions) {
	string userDidSelectOption;
	SDL_StartTextInput();

	while (true){
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0) {
			if(event.type == SDL_KEYDOWN){
				switch(event.key.keysym.sym){
				case SDLK_RETURN:
					for (string posibleOption : posibleOptions) {
						if (strcmp(posibleOption.c_str(), userDidSelectOption.c_str()) == 0) {
							SDL_StopTextInput();
							return userDidSelectOption;
						}
					}
					break;
				case SDLK_BACKSPACE:
					if( userDidSelectOption.size() > 0){
						userDidSelectOption.pop_back();
						this->addinputTextAtLine(userDidSelectOption, 4);
					}
					break;
				}
			}
			switch(event.type){
			case SDL_QUIT:
				SDL_StopTextInput();
				return "";
				break;
			case SDL_TEXTINPUT:
				userDidSelectOption.append(event.text.text);
				this->addinputTextAtLine(userDidSelectOption, 4);
				break;
			}
		}
	}
}

void MenuPresenter::addinputTextAtLine(string text, int line) {
	this->clearTexts(false);
	SDL_RenderCopy(renderer, this->backgroundTexture, NULL, NULL);
	SDL_RenderPresent(renderer);

	for (map<string, string> textToPrint : this->textsToPrint) {
		int lineToPrint = atoi((textToPrint.find("line")->second).c_str());
		this->presentTextAtLine(textToPrint.find("text")->second, lineToPrint, false);
	}
	this->presentTextAtLine(text, line, false);
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
				case SDLK_RETURN:
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

void MenuPresenter::presentCreatTeamOptionMenu() {
	this->clearTexts(true);

	this->presentTextAtLine("Elije una opcion: ", 1, true);
	this->presentTextAtLine("1. Crear equipo", 2, true);
}

void MenuPresenter::presentJoinTeamOptionMenu() {
	this->clearTexts(true);
	this->presentTextAtLine("Elije una opcion: ", 1, true);
}

void MenuPresenter::presentTextAtLine(string text, int line, bool addToList) {
	SDL_Texture *texture = this->textureForText(text);
	SDL_Rect rect = this->rectForLine(line, text);

	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_RenderPresent(renderer);
	if (addToList) {
		this->addTotextsToPrint(text, line);
	}
}

void MenuPresenter::addTotextsToPrint(string text, int line) {
	map <string, string> textToPrint;
	textToPrint["text"] = text;
	textToPrint["line"] = std::to_string(line);
	this->textsToPrint.push_back(textToPrint);
}

void MenuPresenter::presentEnterYourNameText() {
	this->presentTextAtLine("Ingresa tu nombre: ", 1, true);
}

SDL_Texture* MenuPresenter::textureForText(string text) {
	SDL_Color colorText;
	colorText.b = 255;
	colorText.g = 255;
	colorText.r = 255;
	SDL_Surface* textSurface = TTF_RenderText_Solid( this->textFont, text.c_str(), colorText );
	if(textSurface == NULL && text.size() > 0){
		cout<<"Error al cargar la surface del texto"<<endl;
		cout<<TTF_GetError()<<endl;
		return NULL;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if( texture == NULL && text.size() > 0){
		cout<<"Error al cargar la textura del texto"<<endl;
		cout<<SDL_GetError()<<endl;
	}

	SDL_FreeSurface(textSurface);
	return texture;
}

void MenuPresenter::setResultTexture(string result){
	destroyTexture(this->resultMsg);

	this->presentCreatTeamOptionMenu();
}

SDL_Rect MenuPresenter::rectForLine( int line, string text) {
	SDL_Rect firstLineRect{width/2, height/4 + 60,	text.size()*fontSize * 0.5, fontSize};
	SDL_Rect rect {firstLineRect.x, firstLineRect.y + firstLineRect.h * (line - 1),firstLineRect.w, firstLineRect.h};
	return rect;
}

void MenuPresenter::clearTexts(bool clearTexts) {
	SDL_SetRenderDrawColor( renderer, 0x00, 0x00,0x00,0x00);
	SDL_RenderClear( renderer);

	if (clearTexts)
		this->textsToPrint.clear();

	SDL_RenderCopy(renderer, this->backgroundTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
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

