/*
 * Score.h
 *
 *  Created on: 31 de may. de 2016
 *      Author: gusta
 */
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#ifndef SCORE_H_
#define SCORE_H_

using namespace std;

class Score {
public:
	Score();
	int getPoints();
	void setPoints(int points);
	void setName(string name);
	void setPosition(int posx, int posy);
	void paint();
	void setFontType(string fontPath, int fontSize);
	void setRenderer(SDL_Renderer* windowRenderer);
	virtual ~Score();
	void setId(int id){
		this->id = id;
	}
	int getId(){
		return this->id;
	}
	void setTeamId(int id){
		this->teamId = id;
	}
	int getTeamId(){
		return this->teamId;
	}
private:
	string clientName;
	int points;
	SDL_Texture* pointsTexture;
	SDL_Texture* nameTexture;
	TTF_Font* fontType;
	int fontSize;
	SDL_Renderer* renderer;
	int posX;
	int posY;
	int id;
	int teamId;
};

#endif /* SCORE_H_ */
