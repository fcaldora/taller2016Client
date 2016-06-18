#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include <sys/unistd.h>
#include <string.h>
#include "XmlParser.h"
#include "XMLLoader.h"
#include <list>
#include <sys/time.h>
#include "LogWriter.h"
#include <errno.h>
#include <SDL2/SDL.h>
#include <chrono>
#include "Window.h"
#include "Score.h"
#include "StageInfo.h"
#include "Object.h"
#include "MenuPresenter.h"
#include <mutex>
#include "Client.h"
#include "Avion.h"
#include "ScoresManager.h"
#include <SDL2/SDL_mixer.h>
#include "Background.h"
#define MAXHOSTNAME 256
#define kClientTestFile "clienteTest.txt"

using namespace std;

list<Object> objects;

mutex mutexObjects;
XMLLoader *xmlLoader;
XmlParser *parser;
LogWriter *logWriter;
bool userIsConnected;
string nombre;
//Score* myScore;
//Score* theirScore;
ScoresManager* scoresManager;
//list<Score*>::iterator playersIt;
Client* client;
Window* window;
Avion* avion;
int myPlaneId, teamId;
Mix_Music *gMusic = NULL;
Mix_Chunk *fireSound = NULL;
Mix_Chunk *explosionSound = NULL;
StageInfo* stageInfo;
const Uint8* state = SDL_GetKeyboardState(NULL);
MenuPresenter graphicMenu;
bool colaboration;

void putPlaneLastInTheList(){
	list<Object>::iterator it = objects.begin();
	bool found = false;
	while(!found && it != objects.end()){
		if((*it).getId() == myPlaneId)
			found = true;
		else
			it++;
	}
	if(found){
		objects.splice(objects.end(),objects,it);
	}
}

void prepareForExit(XMLLoader *xmlLoader, XmlParser *xmlParser, LogWriter *logWriter) {
	delete xmlLoader;
	delete xmlParser;
	delete logWriter;
}

void closeSocket(int socket) {
	close(socket);
	userIsConnected = false;
}


void initializeSDLSounds() {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_AUDIO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    }
    //Initialize SDL_mixer
   if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
       printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
   }

   //Load music
   gMusic = Mix_LoadMUS( "gameMusic.wav" );
   if( gMusic == NULL ) {
       printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
   }
   fireSound = Mix_LoadWAV( "gun-gunshot-01.wav" );

   //If there was a problem loading the sound effects
   if (fireSound == NULL) {
       printf( "Failed to load fire sound. SDL_mixer Error: %s\n", Mix_GetError() );
   }

   explosionSound = Mix_LoadWAV( "explosion.wav" );

   //If there was a problem loading the sound effects
   if (explosionSound == NULL) {
       printf( "Failed to load explosion sound. SDL_mixer Error: %s\n", Mix_GetError() );
   }




}

void playMusic() {
    if( Mix_PlayingMusic() == 0 ) {
		//Play the music
		Mix_PlayMusic( gMusic, -1 );
	}
}

void playFireSound() {
	if( Mix_PlayChannel( -1, fireSound, 0 ) == -1 ) {
		cout<< "Error playing fireSound"<< endl;
	}
}

void playExplosionSound() {
	if( Mix_PlayChannel( -1, explosionSound, 0 ) == -1 ) {
		cout<< "Error playing explosion sound"<< endl;
	}
}

void closeSDLMixer() {
    //Free the sound effects
    Mix_FreeChunk( fireSound );
    fireSound = NULL;
    Mix_FreeChunk( explosionSound );
    fireSound = NULL;

    //Free the music
    Mix_FreeMusic( gMusic );
    gMusic = NULL;
    //Quit SDL subsystems
    Mix_Quit();
}


int initializeClient(string destinationIp, int port) {
	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;
	int socketHandle;
	const char *remoteHost = destinationIp.c_str();
	int portNumber = port;

	bzero(&remoteSocketInfo, sizeof(sockaddr_in));  // Clear structure memory

	// Get system information
	//ip invalida.
	if ((hPtr = gethostbyname(remoteHost)) == NULL) {
		cerr << "System DNS name resolution not configured properly." << endl;
		logWriter->writeConnectionErrorDescription("Error en la IP. System DNS name resolution not configured properly.");
		prepareForExit(xmlLoader, parser, logWriter);
		exit(EXIT_FAILURE);
	}

	// create socket

	if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		close(socketHandle);
		logWriter->writeConnectionErrorDescription("Error creando el socket. SOCKET FAILURE");
		prepareForExit(xmlLoader, parser, logWriter);
		exit(EXIT_FAILURE);
	}

	// Load system information into socket data structures

	memcpy((char *) &remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short) portNumber);  // Set port number

	if (connect(socketHandle, (struct sockaddr *) &remoteSocketInfo,
			sizeof(sockaddr_in)) < 0) {

		close(socketHandle);
		logWriter->writeConnectionErrorDescription("Puede que el servidor este apagado. Intenta mas tarde");
		return 0;
	}

	return socketHandle;
}

int sendMsj(int socket, int bytesAEnviar, clientMsj* mensaje){
	int enviados = 0;
	int res = 0;
	while(enviados<bytesAEnviar){
		res = send(socket, &(mensaje)[enviados], bytesAEnviar - enviados, MSG_WAITALL);
		if (res == 0){
			logWriter->writeErrorConnectionHasClosed();
			userIsConnected = false;
			return 0;
		}else if(res<0){
			logWriter->writeErrorInSendingMessage(mensaje);
			return -1;
		}else if (res > 0){
			enviados += res;
		}
	}
	return enviados;
}

void sendMenuMessage (int socket, int bytesToSend, menuRequestMessage *message) {
	int bytesSent = 0;
	int res = 0;

	while(bytesSent < bytesToSend){
		res = send(socket, &(message)[bytesSent], bytesToSend - bytesSent, MSG_WAITALL);
		if (res == 0){
			logWriter->writeErrorConnectionHasClosed();
			userIsConnected = false;
		}else if (res > 0){
			bytesSent += res;
		}
	}
}

char* readMsj(int socket, int bytesARecibir, clientMsj* msj){
	int totalBytesRecibidos = 0;
	int recibidos = 0;
	while (totalBytesRecibidos < bytesARecibir){
		recibidos = recv(socket, &msj[totalBytesRecibidos], bytesARecibir - totalBytesRecibidos, MSG_WAITALL);
		if (recibidos < 0){
			logWriter->writeErrorInReceivingMessageWithID(msj->id);
			userIsConnected = false;
			return "";
		}else if(recibidos == 0){
				close(socket);
				userIsConnected = false;
				logWriter->writeErrorConnectionHasClosed();
				return "";
		}else{
			totalBytesRecibidos += recibidos;
		}
	}

	logWriter->writeReceivedSuccessfullyMessage(msj);
	return msj->type;
}

void readStatsTypeMessage(int socket, int bytesToReceive, StatsTypeMessage *message) {
	int totalBytesReceived = 0;
	int currentBytesReceived = 0;

	while (totalBytesReceived < bytesToReceive){
		currentBytesReceived = recv(socket, &message[totalBytesReceived], bytesToReceive - totalBytesReceived, MSG_WAITALL);
		if (currentBytesReceived < 0){
			userIsConnected = false;
		}else if(currentBytesReceived == 0){
			close(socket);
			userIsConnected = false;
			logWriter->writeErrorConnectionHasClosed();
		}else{
			totalBytesReceived += currentBytesReceived;
		}
	}
}

void readCollaborationStatsMessage(int socket, int bytesToReceive, CollaborationStatsMessage *message) {
	int totalBytesReceived = 0;
	int currentBytesReceived = 0;

	while (totalBytesReceived < bytesToReceive){
		currentBytesReceived = recv(socket, &message[totalBytesReceived], bytesToReceive - totalBytesReceived, MSG_WAITALL);
		if (currentBytesReceived < 0){
			userIsConnected = false;
		}else if(currentBytesReceived == 0){
			close(socket);
			userIsConnected = false;
			logWriter->writeErrorConnectionHasClosed();
		}else{
			totalBytesReceived += currentBytesReceived;
		}
	}
}

void readTeamsStatsMessage(int socket, int bytesToReceive, TeamsStatsMessage *message) {
	int totalBytesReceived = 0;
	int currentBytesReceived = 0;

	while (totalBytesReceived < bytesToReceive){
		currentBytesReceived = recv(socket, &message[totalBytesReceived], bytesToReceive - totalBytesReceived, MSG_WAITALL);
		if (currentBytesReceived < 0){
			userIsConnected = false;
		}else if(currentBytesReceived == 0){
			close(socket);
			userIsConnected = false;
			logWriter->writeErrorConnectionHasClosed();
		}else{
			totalBytesReceived += currentBytesReceived;
		}
	}
}

void readMenuMessage(int socket, int bytesToReceive ,menuResponseMessage *message){
	int totalBytesReceived = 0;
	int currentBytesReceived = 0;

	while (totalBytesReceived < bytesToReceive){
		currentBytesReceived = recv(socket, &message[totalBytesReceived], bytesToReceive - totalBytesReceived, MSG_WAITALL);
		if (currentBytesReceived < 0){
			userIsConnected = false;
		}else if(currentBytesReceived == 0){
				close(socket);
				userIsConnected = false;
				logWriter->writeErrorConnectionHasClosed();
		}else{
			totalBytesReceived += currentBytesReceived;
		}
	}
}

int readObjectMessage(int socket, int bytesARecibir, mensaje* msj){
	int totalBytesRecibidos = 0;
	int recibidos = 0;
	while (totalBytesRecibidos < bytesARecibir){
		recibidos = recv(socket, &msj[totalBytesRecibidos], bytesARecibir - totalBytesRecibidos, MSG_WAITALL);
		if (recibidos < 0){
			userIsConnected = false;
			return recibidos;
		}else if(recibidos == 0){
				close(socket);
				userIsConnected = false;
				logWriter->writeErrorConnectionHasClosed();
				return recibidos;
		}else{
			totalBytesRecibidos += recibidos;
		}
	}
	return recibidos;
}

void initializeSDL(int socketConnection, mensaje windowMsj){
	window = new Window("1942", windowMsj.height, windowMsj.width);
	window->paint();
}

void updateObject(mensaje msj){
	list<Object>::iterator iterador;
	for (iterador = objects.begin(); iterador != objects.end(); iterador++){
		if((*iterador).getId() == msj.id ){
			(*iterador).setPosX(msj.posX);
			(*iterador).setPosY(msj.posY);
			(*iterador).setActualPhotogram(msj.actualPhotogram);
		}
	}
}

void deleteObject(mensaje msj){
	list<Object>::iterator iterador = objects.begin();
	bool borrado = false;
	while (!borrado && iterador != objects.end()){
		if((*iterador).getId() == msj.id ){
			(*iterador).destroyTexture();
			objects.erase(iterador);
			borrado = true;
		}else
			iterador++;
	}
}

void changePath(mensaje msj){
	list<Object>::iterator iterador;
	for (iterador = objects.begin(); iterador != objects.end(); iterador++){
		if((*iterador).getId() == msj.id ){
			(*iterador).setPath(msj.imagePath);
			(*iterador).loadImage(msj.imagePath,  window->getRenderer(), msj.width, msj.height);
			cout << "CAMBIA LA IMAGEN" << endl;
		}
	}
}

void createObject(mensaje msj){
	Object object;
	object.setHeight(msj.height);
	object.setWidth(msj.width);
	object.setId(msj.id);
	object.setPosX(msj.posX);
	object.setPosY(msj.posY);
	object.setActualPhotogram(msj.actualPhotogram);
	object.setPhotograms(msj.photograms);
	object.setPath(msj.imagePath);
	object.loadImage(msj.imagePath, window->getRenderer(), msj.width, msj.height);
	objects.push_back(object);
}

void handleEvents(int socket) {
	SDL_Event event;
	clientMsj msg;
	list<int> eventsToSend;
	bool spaceBarPressed = false;
	bool rPressed = false;

	while (userIsConnected) {
		usleep(10000);
		SDL_PumpEvents();
		if (state[SDL_SCANCODE_DOWN])
			eventsToSend.push_back(1);
		if (state[SDL_SCANCODE_UP])
			eventsToSend.push_back(2);
		if (state[SDL_SCANCODE_RIGHT])
			eventsToSend.push_back(3);
		if (state[SDL_SCANCODE_LEFT])
			eventsToSend.push_back(4);
		if (state[SDL_SCANCODE_A])
			eventsToSend.push_back(7);
		if (state[SDL_SCANCODE_X])
			eventsToSend.push_back(8);
		if(state[SDL_SCANCODE_P])
			eventsToSend.push_back(10);
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_SPACE:
						if (!spaceBarPressed) {
							eventsToSend.push_back(5);
							spaceBarPressed = true;
						}
						break;
					case SDLK_r:
						if (!rPressed) {
							cout << "r presionada" << endl;
							eventsToSend.push_back(6);
							rPressed = true;
						}
						break;
				}
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
					case SDLK_SPACE:
						spaceBarPressed = false;
						break;
					case SDLK_r:
						rPressed = false;
						break;
				}
			} else if (event.type == SDL_QUIT) {
				eventsToSend.push_back(8);
			}
		}

		while (eventsToSend.size() > 0) {
			int event = eventsToSend.front();
			strcpy(msg.id, nombre.c_str());
			switch (event) {
			case 1:
				strcpy(msg.type, "movement");
				strcpy(msg.value, "ABJ");
				break;
			case 2:
				strcpy(msg.type, "movement");
				strcpy(msg.value, "ARR");
				break;
			case 3:
				strcpy(msg.type, "movement");
				strcpy(msg.value, "DER");
				break;
			case 4:
				strcpy(msg.type, "movement");
				strcpy(msg.value, "IZQ");
				break;
			case 5:
				strcpy(msg.type, "shoot");
				strcpy(msg.value, "DIS");
				break;
			case 6:
				strcpy(msg.type, "reset");
				strcpy(msg.value, "RES");
				break;
			case 7:
				strcpy(msg.type, "animation");
				strcpy(msg.value, "ANIMATE");
				break;
			case 8:
				userIsConnected = false;
				break;
			case 9:
				strcpy(msg.type, "close");
				strcpy(msg.value, "CLOSE");
				break;
			case 10:
				strcpy(msg.type, "practiceOff");
				strcpy(msg.value, "PRACTICEOFF");
				break;
			}
			sendMsj(socket, sizeof(msg), &msg);
			eventsToSend.pop_front();
		}
		usleep(10000);
	}
}

void keepAlive(int socketConnection){
	clientMsj msg;
	memset(&msg, 0, sizeof(clientMsj));
	strcpy(msg.type, "alive");
	while(userIsConnected){
		usleep(1000);
		sendMsj(socketConnection, sizeof(msg), &msg);
	}
}

void draw(){
	mutexObjects.lock();
	if(objects.size()>0){
		window->paintAll(objects);
	}
	scoresManager->paint();
	if(stageInfo->paintNow())
		stageInfo->paint();
	mutexObjects.unlock();
	window->paint();
}

void resetAll(){
	list<Object>::iterator it = objects.begin();
	for(; it != objects.end(); it++){
		(*it).destroyTexture();
	}
	objects.clear();
}

void createTeamScore(mensaje msj){
	Score* newClientScore = new Score();
	newClientScore->setRenderer(window->getRenderer());
	newClientScore->setFontType("Caviar_Dreams_Bold.ttf",10);
	string name(msj.imagePath);
	cout << "CREANDO PUNTAJE PARA: "<< name << endl;
	newClientScore->setName(name);
	newClientScore->setPoints(0);
	newClientScore->setPosition(window->getWidth() - window->getWidth()/(msj.id +1), window->getHeight() - window->getHeight()/8);
	newClientScore->paint();
	newClientScore->setTeamId(msj.id);
	scoresManager->addScore(newClientScore);
}

void createScore(mensaje msj){
	Score* newClientScore = new Score();
	newClientScore->setRenderer(window->getRenderer());
	newClientScore->setFontType("Caviar_Dreams_Bold.ttf",10);
	string name(msj.imagePath);
	cout << "CREANDO PUNTAJE PARA: "<< name << endl;
	newClientScore->setName(name);
	newClientScore->setPoints(0);
	newClientScore->setPosition((window->getWidth()/6)*msj.id, window->getHeight() - window->getHeight()/8);
	newClientScore->paint();
	newClientScore->setId(msj.id);
	newClientScore->setTeamId(msj.height);
	scoresManager->addScore(newClientScore);
}

void presentCollaborationStats(int socket) {
	CollaborationStatsMessage message;
	readCollaborationStatsMessage(socket, sizeof(message), &message);
	char *str1 = "El jugador de mayor puntaje es: ";
	char *bestPlayerName = message.bestPlayerName;
	char *str2 = " con: ";
	int number = message.bestPlayerScore;
	stringstream strs;
	strs << number;
	string tempStr = strs.str();
	char const *bestPlayerScore = tempStr.c_str();
	char *str3 = " puntos.";

	char *str4 = (char *) malloc(1 + strlen(str1)+ strlen(bestPlayerName)+ strlen(str2)+ strlen(bestPlayerScore) + strlen(str3));
	strcpy(str4, str1);
	strcat(str4, bestPlayerName);
	strcat(str4, str2);
	strcat(str4, bestPlayerScore);
	strcat(str4, str3);

	graphicMenu.addTextToTheEnd(str4);
	free(str4);
}

void presentTeamsStats(int socket) {
	TeamsStatsMessage message;
	readTeamsStatsMessage(socket, sizeof(message), &message);
	graphicMenu.presentTeamStatsForMessage(message);
}

void receiveFromSever(int socket){
	mensaje msj;
	while(userIsConnected){
		readObjectMessage(socket, sizeof(msj), &msj);
		mutexObjects.lock();
		if(strcmp(msj.action, "create") == 0){
			createObject(msj);
		}else if(strcmp(msj.action, "draw") == 0){
			updateObject(msj);
		}else if(strcmp(msj.action, "delete") == 0){
			deleteObject(msj);
		}else if(strcmp(msj.action, "path") == 0){
			changePath(msj);
		}else if(strcmp(msj.action, "score") == 0){
				scoresManager->setPoints(msj);
		}else if (strcmp(msj.action, "close")==0){
			StatsTypeMessage message;
			readStatsTypeMessage(socket, sizeof(message), &message);
			stageInfo->setHasToPaint(true);
			stageInfo->setEndGameInfo();
			graphicMenu.presentTheEnd();
			if (strcmp(message.statType, "collaboration") == 0) {
				presentCollaborationStats(socket);
			} else {
				presentTeamsStats(socket);
			}

			userIsConnected = false;
			sleep(5); //PARA QUE SE VEA LA IMAGEN FINAL
		}else if (strcmp(msj.action, "reset") == 0){
			resetAll();
		}else if (strcmp(msj.action, "bulletSound") == 0){
			playFireSound();
		}else if (strcmp(msj.action, "explosionSound") == 0){
					playExplosionSound();
		}else if (strcmp(msj.action, "windowSize") == 0){
			SDL_SetWindowSize(window->window, msj.width, msj.height);
			window->setHeight(msj.height);
			window->setWidth(msj.width);
			window->paint();
		}else if(strcmp(msj.action, "sortPlane") == 0){
			putPlaneLastInTheList();
		}else if(strcmp(msj.action, "endStage") == 0){
			stageInfo->setHasToPaint(true);
			stageInfo->setPointsInfo(msj.photograms);
			stageInfo->setStageInfo(msj.actualPhotogram);
		}else if(strcmp(msj.action, "teamId") == 0){
			teamId = msj.id;
		}else if(strcmp(msj.action, "createScore") == 0){
			createScore(msj);
		}else if(strcmp(msj.action, "createTeamScore") == 0){
			createTeamScore(msj);
		}
		mutexObjects.unlock();
	}
}

void createTeamWithName(string teamName, int destinationSocket, bool firstTeamCreated) {
	menuRequestMessage message;
	message.id = 0;
	strncpy(message.type, kCreateTeamType, kLongChar);
	strncpy(message.teamName, teamName.c_str(), kLongChar);
	sendMenuMessage(destinationSocket, sizeof(message), &message);
}

void joinTeamWithName(char teamName[kLongChar], int destinationSocket) {
	menuRequestMessage message;
	message.id = 0;
	strncpy(message.type, kJoinTeamType, kLongChar);
	strncpy(message.teamName, teamName, kLongChar);
	sendMenuMessage(destinationSocket, sizeof(message), &message);
}

void presentCreateTeamOptionMenu(menuResponseMessage message, int destinationSocket) {
	string optionSelected;
	graphicMenu.presentCreatTeamOptionMenu();
	vector <string> posibleOptions;
	posibleOptions.push_back("1");
	if (message.firstTeamIsAvailableToJoin) {
		string joinTeamText = "2. Unirse al equipo ";
		joinTeamText += message.firstTeamName;
		graphicMenu.presentTextAtLine(joinTeamText, 3, true);
		posibleOptions.push_back("2");
	}

	optionSelected = graphicMenu.presentCreateOrJoinTeamOptionMenuAndGetSelectedOption(posibleOptions);

	if (atoi(optionSelected.c_str()) == 1) {
		graphicMenu.presentTextAtLine(optionSelected, 4, true);
		graphicMenu.presentTextAtLine("Ingrese el nombre del equipo: ", 5, true);

		string teamName = graphicMenu.presentCreateTeamOptionAndGetName();
		createTeamWithName(teamName, destinationSocket, message.firstTeamIsAvailableToJoin);
	}
	if (atoi(optionSelected.c_str()) == 2) {
		joinTeamWithName(message.firstTeamName, destinationSocket);
	}
	graphicMenu.presentTextAtLine("Esperando a otros jugadores...", 7, true);

}

void presentOnlyJoinTeamOptionMenu(menuResponseMessage message, int destinationSocket) {
	string optionSelected;
	graphicMenu.presentJoinTeamOptionMenu();
	vector <string> posibleOptions;
	if (message.firstTeamIsAvailableToJoin){
		string joinTeamText = "1. Unirse al equipo ";
		joinTeamText += message.firstTeamName;
		graphicMenu.presentTextAtLine(joinTeamText, 2, true);
		posibleOptions.push_back("1");
	}
	if (message.secondTeamIsAvailableToJoin) {
		string joinTeamText = "2. Unirse al equipo ";
		joinTeamText += message.secondTeamName;
		graphicMenu.presentTextAtLine(joinTeamText, 3, true);
		posibleOptions.push_back("2");
	}

	optionSelected = graphicMenu.presentCreateOrJoinTeamOptionMenuAndGetSelectedOption(posibleOptions);
	graphicMenu.presentTextAtLine("Esperando a otros jugadores...", 5, true);

	if (atoi(optionSelected.c_str()) == 1) {
		joinTeamWithName(message.firstTeamName, destinationSocket);
	}
	if (atoi(optionSelected.c_str()) == 2) {
		joinTeamWithName(message.secondTeamName, destinationSocket);
	}
}

void presentTeamMenu(int destinationSocket) {
	menuResponseMessage message;
	readMenuMessage(destinationSocket, sizeof(message), &message);
	if (message.userCanCreateATeam) {
		presentCreateTeamOptionMenu(message, destinationSocket);
	} else {
		presentOnlyJoinTeamOptionMenu(message, destinationSocket);
	}
}

int main(int argc, char* argv[]) {
	const char *fileName;
	scoresManager = new ScoresManager();
	logWriter = new LogWriter();
	xmlLoader = new XMLLoader(logWriter);
	userIsConnected = false;
	colaboration = false;
	teamId = -1;
	if(argc != 2){
		fileName = kClientTestFile;
		logWriter->writeUserDidnotEnterFileName();
	} else {
		fileName = argv[1];
		if (!xmlLoader->clientXMLIsValid(fileName)){
			fileName = kClientTestFile;
			xmlLoader->clientXMLIsValid(fileName);
		}
	}

	parser = new XmlParser(fileName);
	logWriter->setLogLevel(parser->getLogLevel());
	string serverIP = parser->getServerIP();
	int serverPort = parser->getServerPort();

	int destinationSocket;
	client = new Client();
	mensaje windowMsj;
	bool sdlInitiated = false;
	while(!userIsConnected){
		destinationSocket = initializeClient(serverIP, serverPort);
		readObjectMessage(destinationSocket, sizeof(windowMsj), &windowMsj);
		if(!sdlInitiated){
			initializeSDL(destinationSocket, windowMsj);
			sdlInitiated = true;
			graphicMenu.setRenderer(window->renderer);
		}
		graphicMenu.loadMenuBackground("fMenu.png",windowMsj.width, windowMsj.height);
		if(graphicMenu.presentNameMenu())
			nombre = graphicMenu.getPlayerName();
		else
			return 0;
		cout<<"Nombre elegido: "<<graphicMenu.getPlayerName()<<endl;

		clientMsj recibido;
		clientMsj inicializacion;
		memset(&recibido, 0, sizeof(clientMsj));
		memset(&inicializacion, 0, sizeof(clientMsj));
		strcpy(inicializacion.value,nombre.c_str());
		sendMsj(destinationSocket,sizeof(inicializacion),&inicializacion);

		char* messageType = readMsj(destinationSocket, sizeof(recibido), &recibido);
		if (strcmp(messageType, kServerFullType) == 0) {
			closeSocket(destinationSocket);
			graphicMenu.setResultTexture(kServerFullType);
			graphicMenu.paint();
			graphicMenu.erasePlayerName();
			logWriter->writeCannotConnectDueToServerFull();
			sleep(3);
			return 0;
		}else if(strcmp(recibido.type,"error") == 0){
			closeSocket(destinationSocket);
			graphicMenu.setResultTexture(recibido.value);
			graphicMenu.paint();
			graphicMenu.erasePlayerName();
			cout<< recibido.value << endl;
			sleep(2);
		} else {
			userIsConnected = true;
			myPlaneId = atoi(recibido.id);
			if(strcmp(recibido.value, "colaboration") == 0){
				colaboration = true;
			}
			if (recibido.isFirstTimeLogin && !colaboration)
				presentTeamMenu(destinationSocket);
		}
	}

	if(userIsConnected){
		mensaje escenarioMsj;
		readObjectMessage(destinationSocket, sizeof(escenarioMsj), &escenarioMsj);
		createObject(escenarioMsj);
		logWriter->writeUserHasConnectedSuccessfully();


		stageInfo = new StageInfo();
		stageInfo->setRenderer(window->getRenderer());
		stageInfo->setFontType("Caviar_Dreams_Bold.ttf", 20);
		stageInfo->setStageInfo(0);
		stageInfo->setPointsInfo(0);
		stageInfo->setPositions(window->getWidth()/2 - 100, window->getHeight()/2 - 100);

		//graphicMenu.~MenuPresenter();
		initializeSDLSounds();
		playMusic();
		client->threadSDL = std::thread(handleEvents, destinationSocket);
		client->threadListen = std::thread(receiveFromSever, destinationSocket);
		client->threadKeepAlive = std::thread(keepAlive, destinationSocket);
	}

	while(userIsConnected){
		draw();
	}
	client->threadSDL.join();
	client->threadListen.join();
	client->threadKeepAlive.join();
	sleep(3); //3 segundos para que se vea el mensaje de fin de juego.
	list<Object>::iterator it;
	for(it = objects.begin(); it != objects.end(); it++){
		(*it).destroyTexture();
	}
	objects.clear();

	SDL_DestroyRenderer(window->getRenderer());
	window->renderer = NULL;
	SDL_DestroyWindow(window->window);
	window->window = NULL;
	IMG_Quit();
	SDL_Quit();
	close(client->getSocketConnection());
	closeSDLMixer();
	logWriter->writeUserDidTerminateApp();
	prepareForExit(xmlLoader, parser, logWriter);

	return EXIT_SUCCESS;
}


