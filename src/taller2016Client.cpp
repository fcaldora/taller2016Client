#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <string.h>
#include "XmlParser.h"
#include "XMLLoader.h"
#include <list>
#include <sys/time.h>
#include <pthread.h>
#include "LogWriter.h"

#define MAXHOSTNAME 256
#define kClientTestFile "clienteTest.txt"

using namespace std;

list<clientMsj*> messagesList;
pthread_t clientThreadID[3];
XMLLoader *xmlLoader;
XmlParser *parser;
LogWriter *logWriter;

struct thread_args {
    int socketConnection;
}args;

void prepareForExit(XMLLoader *xmlLoader, XmlParser *xmlParser, LogWriter *logWriter, string message) {
	cout << message << endl;
	delete xmlLoader;
	delete xmlParser;
	delete logWriter;
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
		logWriter->writeConnectionErrorDescription("Error en la IP");
		//*archivoErrores<<"Error. Ip "<<destinationIp<<" invalida."<<endl;
		prepareForExit(xmlLoader, parser, logWriter, "System DNS name resolution not configured properly.");
		exit(EXIT_FAILURE);
	}

	// create socket

	if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		close(socketHandle);
		logWriter->writeConnectionErrorDescription("Error creando el socket");
		prepareForExit(xmlLoader, parser, logWriter, "SOCKET FAILURE");
		exit(EXIT_FAILURE);
	}

	// Load system information into socket data structures

	memcpy((char *) &remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short) portNumber);  // Set port number

	if (connect(socketHandle, (struct sockaddr *) &remoteSocketInfo,
			sizeof(sockaddr_in)) < 0) {
		close(socketHandle);
		logWriter->writeConnectionErrorDescription("Error en el puerto");
		prepareForExit(xmlLoader, parser, logWriter, "CONNECT FAILURE");
		exit(EXIT_FAILURE);
	}

	cout<<"Connected!"<<endl;
	return socketHandle;
}

int sendMsj(int socket, int bytesAEnviar, clientMsj* mensaje){
	int enviados = 0;
	int res = 0;
	while(enviados<bytesAEnviar){
		res = send(socket, &(mensaje)[enviados], bytesAEnviar - enviados, MSG_WAITALL);
		if (res == 0){
			logWriter->writeErrorConnectionHasClosed();
			return 0;
		}else if(res<0){
			logWriter->writeErrorInSendingMessage(mensaje->id);
			return -1;
		}else{
			enviados += res;
		}
	}
	logWriter->writeMessageSentSuccessfully(mensaje->id);
	return enviados;
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

int readMsj(int socket, int bytesARecibir, clientMsj* msj){
	int totalBytesRecibidos = 0;
	int recibidos = 0;
	while (totalBytesRecibidos < bytesARecibir){
		recibidos = recv(socket, &msj[totalBytesRecibidos], bytesARecibir - totalBytesRecibidos, MSG_WAITALL);
		if (recibidos < 0){
			logWriter->writeErrorInReceivingMessageWithID(msj->id);
			return -1;
		}else if(recibidos == 0){
				close(socket);
				logWriter->writeErrorConnectionHasClosed();
				return -1;
		}else{
			totalBytesRecibidos += recibidos;
		}
	}
	logWriter->writeReceivedSuccessfullyMessageWithID(msj->id);
	return 1;
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
	long double cantidadMilisegundosActual;
	gettimeofday(&tiempoInicial, NULL);
	long double cantidadMilisegundosFinal = (tiempoInicial.tv_sec * 1000) + milisegundos;
	while(!fin){
		clientMsj mensaje;
		parser->getMessage(mensaje, contador);
		sendMsj(socket,20,&mensaje);
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

int main(int argc, char* argv[]) {
	const char *fileName;
	logWriter = new LogWriter();
	xmlLoader = new XMLLoader(logWriter);

	if(argc != 2){
		fileName = kClientTestFile;
		logWriter->writeUserDidnotEnterFileName();
	} else {
		fileName = argv[1];
		if (!xmlLoader->clientXMLIsValid(fileName)){
			fileName = kClientTestFile;
		}
	}

	parser = new XmlParser(fileName);
	logWriter->setLogLevel(parser->getLogLevel());
	string serverIP = parser->getServerIP();
	int serverPort = parser->getServerPort();

	loadMessages(messagesList, parser);
	int destinationSocket;

	printMenu(messagesList);
	unsigned int userDidChooseOption;
	bool appShouldExit = false;
	clientMsj msjDisconnection;
	clientMsj recibido;
	strncpy(msjDisconnection.id, "1",20);
	strncpy(msjDisconnection.type, "STRING",20);
	strncpy(msjDisconnection.value, "Desconectado",20);

	while (!appShouldExit){
		cin>>userDidChooseOption;
		logWriter->writeUserDidSelectOption(userDidChooseOption);
		switch(userDidChooseOption){
			case 1:
				destinationSocket = initializeClient(serverIP, serverPort);
				logWriter->writeUserHasConnectedSuccessfully();
				printMenu(messagesList);
				break;
			case 2:
				sendMsj(destinationSocket, sizeof(msjDisconnection), &msjDisconnection);
				readMsj(destinationSocket, sizeof(recibido), &recibido);
				close(destinationSocket);
				logWriter->writeUserHasDisconnectSuccessfully();
				printMenu(messagesList);
				break;
			case 3:
				appShouldExit = true;
				sendMsj(destinationSocket,sizeof(msjDisconnection), &msjDisconnection);
				readMsj(destinationSocket,sizeof(recibido), &recibido);
				close(destinationSocket);
				break;
			case 4:
				//Ciclar.
				cout<<"Indicar cantidad de milisegundos: "<<endl;
				int milisegundos;
				cin>>milisegundos;
				ciclar(destinationSocket, milisegundos, parser);
				printMenu(messagesList);
				break;
			default:
				if(userDidChooseOption > messagesList.size() + 4){
					cout <<"Opcion incorrecta"<<endl;
				}else{
					clientMsj mensaje;
					parser->getMessage(mensaje, userDidChooseOption - 5);
					sendMsj(destinationSocket, sizeof(mensaje), &mensaje);
					cout<<"mensaje enviado: "<<mensaje.value<<endl;
					readMsj(destinationSocket,sizeof(recibido), &recibido);
					cout<<"Mensaje recibido: "<<recibido.value<<endl;
				}
				printMenu(messagesList);
		}
	}

	logWriter->writeUserDidTerminateApp();
	prepareForExit(xmlLoader, parser, logWriter, "EXIT_SUCCESS");
	return EXIT_SUCCESS;
}
