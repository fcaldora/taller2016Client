/*
 * ScoresManager.h
 *
 *  Created on: Jun 15, 2016
 *      Author: facundo
 */

#ifndef SCORESMANAGER_H_
#define SCORESMANAGER_H_
#include "Score.h"
#include "Constants.h"
#include <mutex>

class ScoresManager {
public:
	ScoresManager();
	virtual ~ScoresManager();
	void setPoints(mensaje msj);
	void addScore(Score* score);
	void paint();
private:
	list<Score*> scores;
	std::mutex mutex;
};

#endif /* SCORESMANAGER_H_ */
