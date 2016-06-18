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

#include "Constants.h"

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
	void presentCreatTeamOptionMenu();
	void presentTextAtLine(string text, int line, bool addToList);
	string presentCreateOrJoinTeamOptionMenuAndGetSelectedOption(vector <string> posibleOptions);
	string presentCreateTeamOptionAndGetName();
	void presentJoinTeamOptionMenu();
	void presentTheEnd();
	void addTextToTheEnd(string text);
	void presentTeamStatsForMessage(TeamsStatsMessage message);

	virtual ~MenuPresenter();

private:
	string playerName;
	list<string> endLines;
	SDL_Texture* inputTexture;
	SDL_Texture* backgroundTexture;
	SDL_Texture* resultMsg;
	SDL_Renderer* renderer;
	TTF_Font* textFont;
	int height;
	int width;
	int fontSize;

	void clearTexts(bool clearTexts);
	SDL_Texture* textureForText(string text);
	void presentEnterYourNameText();
	SDL_Rect rectForTheEnd(int line, string text);
	SDL_Rect rectForLine( int line, string text);
	vector< map < string, string> > textsToPrint;
	void addTotextsToPrint(string text, int line);
	void addinputTextAtLine(string text, int line);
	void presentPlayerStatsText(string playerName, int playerScore);
	void presentTeamStatsText(string statsHeader ,string teamName, int teamScore);
};

#endif /* MENUPRESENTER_H_ */
