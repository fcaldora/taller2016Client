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
#include "Background.h"
#include "Messenger.h"
#define MAXHOSTNAME 256
#define kClientTestFile "clienteTest.txt"

using namespace std;

list<Object> objects;
int cantidadBytesRecibidos = 0;
mutex mutexObjects;
XMLLoader *xmlLoader;
XmlParser *parser;
LogWriter *logWriter;
bool userIsConnected;
string nombre;
Client* client;
Window* window;
Avion* avion;

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

void initializeSDL(int socketConnection, mensaje windowMsj, mensaje escenarioMsj){
	window = new Window("1942", windowMsj.height, windowMsj.width);
	window->paint();
}

void updateObject(updateMsj msj){
	list<Object>::iterator iterador;
	for (iterador = objects.begin(); iterador != objects.end(); iterador++){
		if((*iterador).getId() == msj.id ){
			(*iterador).setPosX(msj.posX);
			(*iterador).setPosY(msj.posY);
			(*iterador).setActualPhotogram(msj.actualPhotogram);
		}
	}
}

void deleteObject(deleteMsj msj){
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
			Messenger().sendClientMsj(socket, sizeof(msg), &msg);
		}
	}

}

void keepAlive(int socketConnection){
	clientMsj msg;
	memset(&msg, 0, sizeof(clientMsj));
	strcpy(msg.type, "alive");
	while(userIsConnected){
		usleep(1000);
		Messenger().sendClientMsj(socketConnection, sizeof(msg), &msg);
	}
}

void draw(){
	mutexObjects.lock();
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
	}
	objects.clear();
}

void receiveFromSever(int socket){
	mensaje msj;
	int res = 0;
	updateMsj updateMsj;
	deleteMsj deleteMsj;
	actionMsj action;
	while(userIsConnected){
		res = Messenger().readActionMsj(socket, sizeof(actionMsj), &action);
		cantidadBytesRecibidos += res;
		mutexObjects.lock();
		if(strcmp(action.action, "create") == 0){
			res = Messenger().readMensaje(socket, sizeof(mensaje), &msj);
			cantidadBytesRecibidos += res;
			createObject(msj);
		}else if(strcmp(action.action, "draw") == 0){
			res = Messenger().readUpdateMsj(socket, sizeof(updateMsj), &updateMsj);
			cantidadBytesRecibidos += res;
			updateObject(updateMsj);
		}else if(strcmp(action.action, "delete") == 0){
			res = Messenger().readDeleteMsj(socket, sizeof(deleteMsj), &deleteMsj);
			cantidadBytesRecibidos += res;
			deleteObject(deleteMsj);
		}else if(strcmp(action.action, "path") == 0){
			res = Messenger().readMensaje(socket, sizeof(mensaje), &msj);
			cantidadBytesRecibidos += res;
			changePath(msj);
		}else if (strcmp(action.action, "close")==0){
			userIsConnected = false;
		}else if (strcmp(action.action, "reset") == 0){
			resetAll();
		}else if (strcmp(action.action, "windowSize") == 0){
			Messenger().readMensaje(socket, sizeof(mensaje), &msj);
			SDL_SetWindowSize(window->window, msj.width, msj.height);
			window->setHeight(msj.height);
			window->setWidth(msj.width);
			window->paint();
		}
		mutexObjects.unlock();
	}
}

// Only for Chano
void syncronizingWithSever(int socket){
	mensaje msj;
	while(userIsConnected){
		Messenger().readMensaje(socket, sizeof(msj), &msj);
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
	while(!userIsConnected){
		destinationSocket = initializeClient(serverIP, serverPort);
		cout << "Ingrese nombre para conectarse: ";
		cin >> nombre;

		clientMsj recibido;
		clientMsj inicializacion;
		memset(&recibido, 0, sizeof(clientMsj));
		memset(&inicializacion, 0, sizeof(clientMsj));
		strcpy(inicializacion.value,nombre.c_str());
		Messenger().sendClientMsj(destinationSocket,sizeof(inicializacion),&inicializacion);

		int res = Messenger().readClientMsj(destinationSocket, sizeof(recibido), &recibido);
		char* messageType = recibido.type;
		if(res<0){
			logWriter->writeErrorInReceivingMessageWithID(recibido.id);
			userIsConnected = false;
		}
		else if(res ==0){
			userIsConnected = false;
			logWriter->writeErrorConnectionHasClosed();
		}else{
			logWriter->writeReceivedSuccessfullyMessage(&recibido);
		}
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
		actionMsj action;
		Messenger().readActionMsj(destinationSocket, sizeof(actionMsj), &action);
		Messenger().readMensaje(destinationSocket, sizeof(windowMsj), &windowMsj);
		Messenger().readActionMsj(destinationSocket, sizeof(actionMsj), &action);
		Messenger().readMensaje(destinationSocket, sizeof(escenarioMsj), &escenarioMsj);
		initializeSDL(destinationSocket, windowMsj, escenarioMsj);
		createObject(escenarioMsj);
		logWriter->writeUserHasConnectedSuccessfully();
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
	cout<<"Cantidad total de bytes recibidos: "<<cantidadBytesRecibidos<<endl;
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
