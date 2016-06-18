/*
 * Constants.h
 *
 *  Created on: Apr 5, 2016
 *      Author: luciano
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <map>

using namespace std;

#define kClientTag "Cliente"
#define kConfigurationTag "configuracion"
#define kLogLevelTag "nivelDeLog"
#define kConnectionTag "conexion"
#define kIPTag "IP"
#define kPortTag "puerto"
#define kMessagesTag "mensajes"
#define kMessageTag "mensaje"
#define kMessageIDTag "id"
#define kMessageTypeTag "tipo"
#define kMessageValueTag "valor"
#define kMaxNumberOfValidPort 65535
#define kMessageTypeInt "INT"
#define kMessageTypeDouble "DOUBLE"
#define kMessageTypeString "STRING"
#define kMessageTypeChar "CHAR"
#define imageErrorPath "question.png"

#define kCreateTeamType "create_team"
#define kJoinTeamType "join_team"

#define kServerFullType "server_full"
#define kConnectionSuccessfulType "connection_successful"

#define kLongChar 20
#define kDefaultPort 8080

enum LogLevelType {
	LogLevelTypeOnlyErrors = 1,
	LogLevelTypeEverything = 2
};

struct clientMsj {
	char id[kLongChar];
	char type[kLongChar];
	char value[kLongChar];
	bool isFirstTimeLogin;
};

struct mensaje {
	char action[kLongChar];
	int id;
	char imagePath[kLongChar];
	int posX;
	int posY;
	int width;
	int height;
	bool activeState;
	int actualPhotogram;
	int photograms;
	char errorMsj[kLongChar];
};

struct menuResponseMessage {
	int id;
	bool userCanCreateATeam;
	char firstTeamName[kLongChar];
	bool firstTeamIsAvailableToJoin;
	char secondTeamName[kLongChar];
	bool secondTeamIsAvailableToJoin;
};

struct StatsTypeMessage {
	int id;
	char statType[kLongChar]; //teams || collaboration
};

struct CollaborationStatsMessage {
	int id;
	char bestPlayerName[kLongChar];
	int bestPlayerScore;
};

struct menuRequestMessage {
	int id;
	char type[kLongChar]; // create_team || join_team
	char teamName[kLongChar];
};

#endif /* CONSTANTS_H_ */
