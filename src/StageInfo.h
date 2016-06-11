/*
 * StageInfo.h
 *
 *  Created on: 3 de jun. de 2016
 *      Author: gusta
 */
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#ifndef STAGEINFO_H_
#define STAGEINFO_H_
#define TIMEPAINT 200
using namespace std;

class StageInfo {
public:
	StageInfo();
	void setHasToPaint(bool hasTo);
	bool paintNow();
	void setPositions(int posx, int posy);
	void setRenderer(SDL_Renderer* windowRenderer);
	void setStageInfo(int numberOfStage);
	void setEndGameInfo();
	void setPointsInfo(int points);
	void setFontType(string path, int fontSize);
	void paint();
	virtual ~StageInfo();

private:
	int posX;
	int posY;
	int paintCounter;
	int fontSize;
	bool hasToPaint;
	TTF_Font* fontType;
	SDL_Renderer* renderer;
	SDL_Texture* stageInfoTexture;
	SDL_Texture* pointsInfoTexture;
	string stageInfo;
	string pointsInfo;
	string fontPath;
};

#endif /* STAGEINFO_H_ */
