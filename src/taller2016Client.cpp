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
Score* myScore;
Score* theirScore;
Client* client;
Window* window;
Avion* avion;
int myPlaneId;
Mix_Music *gMusic = NULL;
Mix_Chunk *fireSound = NULL;
StageInfo* stageInfo;
const Uint8* state = SDL_GetKeyboardState(NULL);



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

void closeSDLMixer() {
    //Free the sound effects
    Mix_FreeChunk( fireSound );
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
	//if (strcmp(msj.imagePath, "bullet.png") == 0) {
	//	playFireSound();
	//}
}

void handleEvents(int socket) {
	SDL_Event event;
	clientMsj msg;
	list<int> eventsToSend;
	bool spaceBarPressed = false;

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
		//if( state[SDL_SCANCODE_SPACE])
		if (state[SDL_SCANCODE_R])
			eventsToSend.push_back(6);
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
				}
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
						spaceBarPressed = false;
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
	myScore->paint();
	theirScore->paint();
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
			if(msj.id == myPlaneId){
				myScore->setPoints(msj.photograms);
				myScore->setPosition(msj.posX, msj.posY);
			}else{
				theirScore->setPoints(msj.photograms);
				theirScore->setPosition(msj.posX, msj.posY);
			}
		}else if (strcmp(msj.action, "close")==0){
			stageInfo->setHasToPaint(true);
			stageInfo->setEndGameInfo();
			userIsConnected = false;
		}else if (strcmp(msj.action, "reset") == 0){
			resetAll();
		}else if (strcmp(msj.action, "bulletSound") == 0){
			playFireSound();
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
		}
		mutexObjects.unlock();
	}
}

// Only for Chano
void syncronizingWithSever(int socket){
	mensaje msj;
	while(userIsConnected){
		readObjectMessage(socket, sizeof(msj), &msj);
		if(strcmp(msj.action, "create") == 0){
			createObject(msj);
		}else if(strcmp(msj.action, "draw") == 0){
			return;
		}else if(strcmp(msj.action, "delete") == 0){
			return;
		}else if(strcmp(msj.action, "path") == 0){
			return;
		}
	}
}
// END Only for Chano


int main(int argc, char* argv[]) {
	const char *fileName;
	logWriter = new LogWriter();
	xmlLoader = new XMLLoader(logWriter);
	userIsConnected = false;

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
	MenuPresenter graphicMenu;
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
		} else {
			userIsConnected = true;
			graphicMenu.setResultTexture("Conected!");
			myPlaneId = atoi(recibido.id);
			graphicMenu.paint();
		}
		sleep(2);
	}
	if(userIsConnected){
		mensaje escenarioMsj;
		readObjectMessage(destinationSocket, sizeof(escenarioMsj), &escenarioMsj);
		createObject(escenarioMsj);
		logWriter->writeUserHasConnectedSuccessfully();

		myScore = new Score();
		myScore->setRenderer(window->getRenderer());
		myScore->setFontType("Caviar_Dreams_Bold.ttf",10);
		myScore->setName(graphicMenu.getPlayerName());
		myScore->setPoints(0);
		myScore->setPosition(0, 0);

		theirScore = new Score();
		theirScore->setRenderer(window->getRenderer());
		theirScore->setFontType("Caviar_Dreams_Bold.ttf",10);
		theirScore->setName("player2");
		theirScore->setPoints(0);
		theirScore->setPosition(0, 0);

		stageInfo = new StageInfo();
		stageInfo->setRenderer(window->getRenderer());
		stageInfo->setFontType("Caviar_Dreams_Bold.ttf", 20);
		stageInfo->setStageInfo(0);
		stageInfo->setPointsInfo(0);
		stageInfo->setPositions(window->getWidth()/2 - 100, window->getHeight()/2 - 100);

		graphicMenu.~MenuPresenter();
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


