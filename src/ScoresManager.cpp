/*
 * ScoresManager.cpp
 *
 *  Created on: Jun 15, 2016
 *      Author: facundo
 */

#include "ScoresManager.h"

ScoresManager::ScoresManager() {
	// TODO Auto-generated constructor stub

}

ScoresManager::~ScoresManager() {
	// TODO Auto-generated destructor stub
}


void ScoresManager::setPoints(mensaje msj){
	list<Score*>::iterator it;
	for(it = scores.begin(); it != scores.end(); it++){
		if((*it)->getId() == msj.actualPhotogram){
			if(msj.id != 0){
				(*it)->setPoints(msj.photograms);
			}else{
				(*it)->setPoints(msj.width);
			}
		}
	}
}

void ScoresManager::addScore(Score* score){
	this->scores.push_back(score);
}

void ScoresManager::paint(){
	list<Score*>::iterator playersIt;
	for(playersIt = scores.begin(); playersIt != scores.end(); playersIt++){
		(*playersIt)->paint();
	}
}
