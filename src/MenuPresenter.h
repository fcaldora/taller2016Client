/*
 * MenuPresenter.h
 *
 *  Created on: 19 de may. de 2016
 *      Author: gusta
 */
#include <string>
#include <sstream>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#ifndef MENUPRESENTER_H_
#define MENUPRESENTER_H_


using namespace std;

class MenuPresenter{
public:
	MenuPresenter();
	void setRenderer(SDL_Renderer* renderer);
	//Devuelve verdadero si ingreso un nombre y apreto enter. Falso si cerro la ventana.
	bool presentNameMenu();
	string getPlayerName();
	void paint();
	SDL_Texture* getResultTexture();
	void loadMenuBackground(string pathImage, int height, int width);
	void destroyTexture(SDL_Texture* texture);
	void setInputTexture();
	void setResultTexture(string result);
	void erasePlayerName();
	virtual ~MenuPresenter();

private:
	string playerName;
	SDL_Texture* inputTexture;
	SDL_Texture* backgroundTexture;
	SDL_Texture* resultMsg;
	SDL_Renderer* renderer;
	TTF_Font* textFont;
	int height;
	int width;
	int fontSize;
};

#endif /* MENUPRESENTER_H_ */
