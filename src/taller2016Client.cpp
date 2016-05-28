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
#include <chrono>
#include "Window.h"
#include "Object.h"
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
Client* client;
Window* window;
Avion* avion;
Mix_Music *gMusic = NULL;


enum MenuOptionChoosedType {
	MenuOptionChoosedTypeConnect = 1,
	MenuOptionChoosedTypeDisconnect = 2,
	MenuOptionChoosedTypeExit = 3,
	MenuOptionChoosedTypeCycle = 4
};

void prepareForExit(XMLLoader *xmlLoader, XmlParser *xmlParser, LogWriter *logWriter) {
	delete xmlLoader;
	delete xmlParser;
	delete logWriter;
}

void closeSocket(int socket) {
	close(socket);
	userIsConnected = false;
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

void initializeSDL(int socketConnection, mensaje windowMsj, mensaje escenarioMsj){
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

void handleEvents(int socket){
	SDL_Event event;
	int button;
	clientMsj msg;
	while(userIsConnected){
		if(SDL_PollEvent( &event) == 1){
			button = avion->processEvent(&event);
		}
		strcpy(msg.id, nombre.c_str());
		switch(button){
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
		}
		if(button != 0 && button != 8){
			usleep(10000);
			sendMsj(socket, sizeof(msg), &msg);
		}
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
	//SDL_RenderClear(window->getRenderer());
	if(objects.size()>0){
		window->paintAll(objects);
	}
	mutexObjects.unlock();
	window->paint();
}

void resetAll(){
	list<Object>::iterator it = objects.begin();
	for(; it != objects.end(); it++){
		(*it).destroyTexture();
		//objects.erase(it);
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
		}else if (strcmp(msj.action, "close")==0){
			userIsConnected = false;
		}else if (strcmp(msj.action, "reset") == 0){
			resetAll();
		}else if (strcmp(msj.action, "windowSize") == 0){
			cout<<"Recibio window size"<<endl;
			SDL_SetWindowSize(window->window, msj.width, msj.height);
			window->setHeight(msj.height);
			window->setWidth(msj.width);
			window->paint();
			cout<<"Seteo todo"<<endl;
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


void initializeSDLSound() {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        //success = false;
    }

    //Initialize SDL_mixer
   if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
   {
       printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
       //success = false;
   }

   //Load music
   gMusic = Mix_LoadMUS( "gameMusic.wav" );
   if( gMusic == NULL )
   {
       printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
       //success = false;
   }


}

void playMusic() {
    if( Mix_PlayingMusic() == 0 ) {
			//Play the music
			Mix_PlayMusic( gMusic, -1 );
			cout << "musica muscia asdfasdf" << endl;
		}
}

void close()
{
    //Free loaded images
    //gPromptTexture.free();

    //Free the sound effects
//    Mix_FreeChunk( gScratch );
//    Mix_FreeChunk( gHigh );
//    Mix_FreeChunk( gMedium );
//    Mix_FreeChunk( gLow );
//    gScratch = NULL;
//    gHigh = NULL;
//    gMedium = NULL;
//    gLow = NULL;
//
    //Free the music
    Mix_FreeMusic( gMusic );
    gMusic = NULL;


    //Quit SDL subsystems
    Mix_Quit();
}

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
	while(!userIsConnected){
		destinationSocket = initializeClient(serverIP, serverPort);
		cout << "Ingrese nombre para conectarse: ";
		cin >> nombre;

		clientMsj recibido;
		clientMsj inicializacion;
		memset(&recibido, 0, sizeof(clientMsj));
		memset(&inicializacion, 0, sizeof(clientMsj));
		strcpy(inicializacion.value,nombre.c_str());
		sendMsj(destinationSocket,sizeof(inicializacion),&inicializacion);

		char* messageType = readMsj(destinationSocket, sizeof(recibido), &recibido);
		if (strcmp(messageType, kServerFullType) == 0) {
			closeSocket(destinationSocket);
			logWriter->writeCannotConnectDueToServerFull();
		}else if(strcmp(recibido.type,"error") == 0){
			closeSocket(destinationSocket);
			cout<< recibido.value << endl;
		} else {
			userIsConnected = true;
		}
	}
	if(userIsConnected){
		mensaje windowMsj, escenarioMsj;
		readObjectMessage(destinationSocket, sizeof(windowMsj), &windowMsj);
		readObjectMessage(destinationSocket, sizeof(escenarioMsj), &escenarioMsj);
		initializeSDL(destinationSocket, windowMsj, escenarioMsj);
		createObject(escenarioMsj);
		logWriter->writeUserHasConnectedSuccessfully();
		initializeSDLSound();
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

	logWriter->writeUserDidTerminateApp();
	prepareForExit(xmlLoader, parser, logWriter);

	return EXIT_SUCCESS;
}


