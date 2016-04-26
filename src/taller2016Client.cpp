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
#include "Client.h"
#include "Avion.h"
#include "Background.h"
#define MAXHOSTNAME 256
#define kClientTestFile "clienteTest.txt"

using namespace std;

list<clientMsj*> messagesList;
list<clientMsj> messagesToSend;

list<Object*> objects;

XMLLoader *xmlLoader;
XmlParser *parser;
LogWriter *logWriter;
bool userIsConnected;
string nombre;
Client* client;
Window* window;
Background* background;
Avion* avion;
int scrollingOffset = 0;

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

//Esta funcion va en la opcion del menu que dice "conectar".
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
		//*archivoErrores<<"Error. Ip "<<destinationIp<<" invalida."<<endl;
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
	struct timeval timeOut;
	timeOut.tv_sec = 100;
	timeOut.tv_usec = 0;
	//if(setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(struct timeval))!=0)
		cout<<"Error al settear el timeout"<<endl;
		cout<<strerror(errno)<<endl;
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
			//logWriter->writeMessageSentSuccessfully(mensaje);
		}
	}
	return enviados;
}

void *desencolarMensajesAenviar(int socket){
	while (1){
			if(!messagesToSend.empty()){
				clientMsj mensaje = messagesToSend.front();
				sendMsj(socket, sizeof(mensaje),&mensaje);
				messagesToSend.pop_front();
			}
		}
}

void printMenu(list<clientMsj*> listaMensajes){
	cout<<"1. Conectar"<<endl;
	cout<<"2. Desconectar"<<endl;
	cout<<"3. Salir "<<endl;
	cout<<"4. Ciclar"<<endl;
	int i=0;
	list<clientMsj*>::iterator iterador;
	for (iterador = listaMensajes.begin(); iterador != listaMensajes.end(); iterador++){
		cout<< i+5 <<". Enviar mensaje "<<(*iterador)->id<<endl;
		i++;
	}
	cout<<"Ingresar opcion: ";
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



void loadMessages(list<clientMsj*> &listaMensajes, XmlParser *parser){
	int cantidadMensajes = parser->getNumberOfMessages();
	int contador;
	for(contador = 0; contador<cantidadMensajes;contador++){
		clientMsj* mensaje = (clientMsj*)malloc(sizeof(clientMsj));
		parser->getMessage(*mensaje, contador);
		listaMensajes.push_back(mensaje);
	}
}

void ciclar(int socket, int milisegundos, XmlParser *parser){
	struct timeval tiempoInicial, tiempoActual;
	bool fin = false;
	int contador = 0;
	int cantidadMensajesEnviados = 0;
	int res = 0;
	long double cantidadMilisegundosActual;
	gettimeofday(&tiempoInicial, NULL);
	long double cantidadMilisegundosFinal = (tiempoInicial.tv_sec * 1000) + milisegundos;
	clientMsj mensaje, recibido;
	while(!fin){
		parser->getMessage(mensaje, contador);
		res = sendMsj(socket,sizeof(clientMsj),&mensaje);
		if(res <= 0)
			return; //Hubo un error. Hay que cerrar esta conexion.
		readMsj(socket,sizeof(clientMsj), &recibido);
		cantidadMensajesEnviados++;
		contador++;
		if(contador == parser->cantidadMensajes())
			contador = 0;
		gettimeofday(&tiempoActual, NULL);
		cantidadMilisegundosActual = tiempoActual.tv_sec*1000;
		if( cantidadMilisegundosActual >= cantidadMilisegundosFinal){
			fin = true;
		}
	}
	cout<<"Cantidad total de mensajes enviados: "<<cantidadMensajesEnviados<<endl;
}



void initializeSDL(int socketConnection){
	window = new Window("Prueba", 640, 480);
	background = new Background();
	background->loadBackground("fondo.png", window->getRenderer());
	SDL_RenderClear(window->getRenderer());
	background->paint(window->getRenderer(),0,0);
	window->paint();
	/*SDL_Event event;
	int end = false;
	clientMsj msg;
	Position pos;
	int button;
	while (userIsConnected){
		scrollingOffset++;
		SDL_RenderClear(window->getRenderer());
		background.paint(window->getRenderer(), 0, scrollingOffset);
		background.paint(window->getRenderer(), 0, scrollingOffset - background.getWidth());
		avion->paint(window->getRenderer(), 240, 320);
		window->paint();
		if(scrollingOffset > background.getWidth())
			scrollingOffset = 0;
		while(SDL_PollEvent( &event ) != 0){
			button = avion->processEvent(&event);
		}
		strcpy(msg.id, nombre.c_str());
		strcpy(msg.type, "movement");
		switch(button){
			case 1:
				strcpy(msg.value, "1");
				break;
			case 2:
				strcpy(msg.value, "2");
				break;
			case 3:
				strcpy(msg.value, "3");
				break;
			case 4:
				strcpy(msg.value, "4");
				break;
			case 5:
				strcpy(msg.value, "5");
				break;
			case -1:
				userIsConnected = false;
				close(client->getSocketConnection());
				client->threadSDL.detach();
		}
		if(button != 0){
			sendMsj(socketConnection, sizeof(msg), &msg);
			readMsj(socketConnection, sizeof(msg), &msg);
		}
	}*/
}
void updateObject(mensaje msj){
	list<Object*>::iterator iterador;
	for (iterador = objects.begin(); iterador != objects.end(); iterador++){
		if((*iterador)->getId() == msj.id ){
			(*iterador)->setPosX(msj.posX);
			(*iterador)->setPosY(msj.posY);
		}
	}
}
void createObject(mensaje msj){
	Object* object = new Object();
	object->setHeight(msj.height);
	object->setWidth(msj.width);
	object->setId(msj.id);
	object->setPosX(msj.posX);
	object->setPosY(msj.posY);
	object->setPhotograms(msj.actualPhotogram);
	object->setPath(msj.imagePath);
	string path(msj.imagePath);
	object->loadImage("avionPrueba1.png", window->getRenderer(), msj.width, msj.height);
	objects.push_back(object);
}

void handleEvents(int socket){
	SDL_Event event;
	int button;
	clientMsj msg;
	mensaje msj;
	while(userIsConnected){
		while(SDL_PollEvent( &event) != 0){
			button = avion->processEvent(&event);
		}
		strcpy(msg.id, nombre.c_str());
		strcpy(msg.type, "movement");
		switch(button){
			case 1:
				strcpy(msg.value, "ABJ");
				break;
			case 2:
				strcpy(msg.value, "ARR");
				break;
			case 3:
				strcpy(msg.value, "DER");
				break;
			case 4:
				strcpy(msg.value, "IZQ");
				break;
			case 5:
				strcpy(msg.value, "DIS");
				break;
			case -1:
				userIsConnected = false;
				close(client->getSocketConnection());
		}
		if(button != 0){
			usleep(1000);
			sendMsj(socket, sizeof(msg), &msg);
			/*readObjectMessage(socket, sizeof(msj), &msj);
			if(strcmp(msj.action, "create") == 0){
				cout << "Creo un avion con id: " << msj.id << endl;
				createObject(msj);
			}else if(strcmp(msj.action, "draw") == 0){
				updateObject(msj);
			}*/
		}
	}

}

void draw(){
	scrollingOffset++;
	SDL_RenderClear(window->getRenderer());
	background->paint(window->getRenderer(), 0, scrollingOffset*0.1 -640);
	background->paint(window->getRenderer(), 0, scrollingOffset*0.1 - background->getHeight());
	list<Object*>::iterator iterador;
	for (iterador = objects.begin(); iterador != objects.end(); iterador++){
		(*iterador)->paint(window->getRenderer(), (*iterador)->getPosX(), (*iterador)->getPosY());
	}
	window->paint();
	if(scrollingOffset*0.1 > background->getHeight())
		scrollingOffset = 0;
}




void receiveFromSever(int socket){
	mensaje msj;
	while(userIsConnected){
		readObjectMessage(socket, sizeof(msj), &msj);
		if(strcmp(msj.action, "create") == 0){
			cout << "Creo un avion con id: " << msj.id << endl;
			createObject(msj);
		}else if(strcmp(msj.action, "draw") == 0){
			updateObject(msj);
		}
	}
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

	loadMessages(messagesList, parser);
	int destinationSocket;
	client = new Client();
	printMenu(messagesList);
	unsigned int userDidChooseOption;
	bool appShouldExit = false;
	while(!userIsConnected){
		destinationSocket = initializeClient(serverIP, serverPort);
		cout << "Ingrese nombre para conectarse.";
		cin >> nombre;

		clientMsj recibido;
		clientMsj inicializacion;
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
			initializeSDL(destinationSocket);
			logWriter->writeUserHasConnectedSuccessfully();
		}
	}
	if(userIsConnected){
		mensaje msj;
		readObjectMessage(destinationSocket, sizeof(msj), &msj);
		createObject(msj);
		client->threadSDL = std::thread(handleEvents, destinationSocket);
		client->threadListen = std::thread(receiveFromSever, destinationSocket);
	}
	while(userIsConnected){
		draw();
	}
	//std::thread desencolarMensajesThread;
	/*while (!appShouldExit){
		cin>>userDidChooseOption;
		logWriter->writeUserDidSelectOption(userDidChooseOption);

		switch(userDidChooseOption){
			case MenuOptionChoosedTypeConnect:
				if (!userIsConnected) {
					destinationSocket = initializeClient(serverIP, serverPort);
					if (destinationSocket > 0) {
						cout << "Ingrese nombre para conectarse.";
						cin >> nombre;

						clientMsj inicializacion;
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
							client->threadSDL =  std::thread(initializeSDL,destinationSocket);
							logWriter->writeUserHasConnectedSuccessfully();
						}
					}

					//desencolarMensajesThread = std::thread(desencolarMensajesAenviar, destinationSocket);

				} else
					cout << "Ya estas conectado!!" << endl;
				printMenu(messagesList);
				break;
			case MenuOptionChoosedTypeDisconnect:
				if (userIsConnected) {
					closeSocket(destinationSocket);
					client->threadSDL.detach();
					logWriter->writeUserHasDisconnectSuccessfully();
				} else
					cout << "Tenes que conectarte primero" << endl;
				printMenu(messagesList);
				break;
			case MenuOptionChoosedTypeExit:
				appShouldExit = true;
				close(destinationSocket);
				client->threadSDL.detach();
				delete client;
				//desencolarMensajesThread.detach();
				break;
			case MenuOptionChoosedTypeCycle:
				if (userIsConnected) {
					cout<<"Indicar cantidad de milisegundos: "<<endl;
					int milisegundos;
					cin>>milisegundos;
					ciclar(destinationSocket, milisegundos, parser);
				} else
					cout << "Tenes que conectarte primero" << endl;

				printMenu(messagesList);
				break;
			default:
				if(userDidChooseOption > messagesList.size() + 4){
					cout <<"Opcion incorrecta"<<endl;
				}else if (userIsConnected){
					clientMsj mensaje;
					parser->getMessage(mensaje, userDidChooseOption - 5);
					//messagesToSend.push_back(mensaje);
					sendMsj(destinationSocket, sizeof(mensaje),&mensaje);
					readMsj(destinationSocket, sizeof(recibido), &recibido);
				} else
					cout << "Primero tenes que conectarte" << endl;
				printMenu(messagesList);
		}
	}*/

	client->threadSDL.detach();
	client->threadListen.detach();

	for(int i = 0; i<parser->getNumberOfMessages();i++){
		free(messagesList.front());
		messagesList.pop_front();
	}
	logWriter->writeUserDidTerminateApp();
	prepareForExit(xmlLoader, parser, logWriter);


	return EXIT_SUCCESS;
}
