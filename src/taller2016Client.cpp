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
#include "CargadorXML.h"
#include <list>
#include <pthread.h>

#define MAXHOSTNAME 256
#define kClientTestFile "clienteTest.txt"

using namespace std;

list<clientMsj*> messagesList;
pthread_t clientThreadID[3];
XMLLoader *xmlLoader;
XmlParser *parser;

struct thread_args {
    int socketConnection;
}args;

void prepareForExit(XMLLoader *xmlLoader, XmlParser *xmlParser, string message) {
	cout << message << endl;
	delete xmlLoader;
	delete xmlParser;
}

//Esta funcion va en la opcion del menu que dice "conectar".
int initializeClient(string destinationIp, int port, ofstream* archivoErrores) {
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
		*archivoErrores<<"Error. Ip "<<destinationIp<<" invalida."<<endl;
		prepareForExit(xmlLoader, parser, "System DNS name resolution not configured properly.");
		exit(EXIT_FAILURE);
	}

	// create socket

	if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		close(socketHandle);
		prepareForExit(xmlLoader, parser, "SOCKET FAILURE");
		exit(EXIT_FAILURE);
	}

	// Load system information into socket data structures

	memcpy((char *) &remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short) portNumber);  // Set port number

	if (connect(socketHandle, (struct sockaddr *) &remoteSocketInfo,
			sizeof(sockaddr_in)) < 0) {
		close(socketHandle);
		*archivoErrores<<"Error. Puerto "<<portNumber<<" inválido."<<endl;
		prepareForExit(xmlLoader, parser, "CONNECT FAILURE");
		exit(EXIT_FAILURE);
	}

	return socketHandle;
}

int write_socket(int destinationSocket, void *buf, int len, ofstream* archivoErrores) {
	int currentsize = 0;
	int count = 0;
	while (currentsize < len) {
		count = write(destinationSocket, buf + currentsize, 1);
		if (count < 0){
			return -1;
			*archivoErrores<<"Error al enviar mensaje."<<endl;
		}
		currentsize += 1;
	}
	return currentsize;
}

void printMenu(list<clientMsj*> listaMensajes){
	cout << endl << "Menu:" << endl;
	cout<<"1. Conectar"<<endl;
	cout<<"2. Desconectar"<<endl;
	cout<<"3. Salir "<<endl;
	int i=0;
	list<clientMsj*>::iterator iterador;
	for (iterador = listaMensajes.begin(); iterador != listaMensajes.end(); iterador++){
		cout<< i+4 <<". Enviar mensaje "<<(*iterador)->id<<endl;
		i++;
	}
	cout << "Ingresar opcion:" ;
}

int readMsj(int socket, int bytesARecibir, ofstream* archivoErrores){
	int totalBytesRecibidos = 0;
	int recibidos = 0;
	int tamBuffer = bytesARecibir;
	char buffer[tamBuffer];
	memset(buffer,0,tamBuffer);
	while (totalBytesRecibidos < bytesARecibir){
		recibidos = recv(socket, &buffer[totalBytesRecibidos], tamBuffer - totalBytesRecibidos, MSG_WAITALL);
		if (recibidos < 0){
			*archivoErrores<<"Error al recibir mensaje"<<endl;
			return -1;
		}else if(recibidos == 0){//se corto la conexion desde el lado del servidor.
				close(socket);
				*archivoErrores<<"Se cerro la conexion con el servidor"<<endl;
				return -1;
		}else{
			totalBytesRecibidos += recibidos;
		}
	}
	cout<<buffer<<endl;
	return 0;
}

int readBlock(int fd, void* buffer, int len, ofstream* archivoErrores) {
	int ret = 0;
	int count = 0;
	while (count < len) {
		ret = read(fd, buffer + count, 1);
		if (ret <= 0) {
			*archivoErrores<<"Error al recibir mensaje"<<endl;
			return (-1);
		}else if(ret == 0){//se corto la conexion desde el lado del servidor.
			close(fd);
			*archivoErrores<<"Se cerro la conexion con el servidor"<<endl;
			return -1;
		}
		count += 1;
	}
	return count;
}

void cargarMensajes(list<clientMsj*> &listaMensajes, XmlParser *parser){
	int cantidadMensajes = parser->getNumberOfMessages();
	int contador;
	for(contador = 0; contador<cantidadMensajes;contador++){
		clientMsj* mensaje = (clientMsj*)malloc(sizeof(clientMsj));
		parser->getMessage(*mensaje, contador);
		listaMensajes.push_back(mensaje);
	}
}

int main(int argc, char* argv[]) {
	const char *fileName;
	xmlLoader = new XMLLoader();

	if(argc != 2){
		fileName = kClientTestFile;
		cout<<"Falta escribir el nombre del archivo del cliente, se usara uno por defecto."<<endl;
	} else {
		fileName = argv[1];
		if (!xmlLoader->clientXMLIsValid(fileName)){
			fileName = kClientTestFile;
		}
	}

	ofstream erroresXml; //Log de errores de mala escritura del XML.
	ofstream erroresConexion;
	erroresConexion.open("ErroresConexion",ios_base::app);
	ofstream erroresDatos;
	erroresDatos.open("erroresEnDatos",ios_base::app);
	parser = new XmlParser(fileName, &erroresXml);
	string serverIP = parser->getServerIP();
	int serverPort = parser->getServerPort();

	cargarMensajes(messagesList, parser);
	int destinationSocket;

	printMenu(messagesList);
	unsigned int opcion;
	bool fin = false;
	while (!fin){
		cin>>opcion;
		clientMsj disconnection, response;
		switch(opcion){
			case 1:
				destinationSocket = initializeClient(serverIP, serverPort, &erroresConexion);
				//avisar si se conecto bien o no.
				printMenu(messagesList);
				break;
			case 2:
				strncpy(disconnection.id, "1", sizeof(disconnection.id) - 1);
				strncpy(disconnection.type, "String", sizeof(disconnection.type) - 1);
				strncpy(disconnection.value, "Disconnection", sizeof(disconnection.value) - 1);
				write_socket(destinationSocket, &disconnection, sizeof(disconnection), &erroresConexion);
				readBlock(destinationSocket, &response, sizeof(response), &erroresConexion);
				close(destinationSocket);
				printMenu(messagesList);
				break;
			case 3:
				fin = true;
				strncpy(disconnection.id, "1", sizeof(disconnection.id) - 1);
				strncpy(disconnection.type, "String", sizeof(disconnection.type) - 1);
				strncpy(disconnection.value, "Disconnection", sizeof(disconnection.value) - 1);
				write_socket(destinationSocket, &disconnection, sizeof(disconnection), &erroresConexion);
				readBlock(destinationSocket, &response, sizeof(response), &erroresConexion);
				close(destinationSocket);
				break;
			default:
				if(opcion > messagesList.size() + 3){
					cout <<"Opcion incorrecta"<<endl;
				}else{
					clientMsj mensaje, response;
					parser->getMessage(mensaje, opcion - 4);
					int msjLength = sizeof(mensaje);
					write_socket(destinationSocket, &mensaje, msjLength, &erroresConexion);
					readBlock(destinationSocket, &response, sizeof(response),  &erroresConexion);
					cout << response.value << endl;
				}
				printMenu(messagesList);
		}
	}

	prepareForExit(xmlLoader, parser, "EXIT_SUCCESS");
	return EXIT_SUCCESS;
}
