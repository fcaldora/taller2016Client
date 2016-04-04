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
using namespace std;

list<clientMsj*> messagesList;
pthread_t clientThreadID[3];

struct thread_args {
    int socketConnection;
}args;

/*int readBlock(int fd, void* buffer, int len) {
	int ret = 0;
	int count = 0;
	while (count < len) {
		ret = read(fd, buffer + count, 1);
		if (ret == 0) {
			return 0;
		}
		if (ret < 0){
			return -1;
		}
		count += 1;
	}
	return count;
}*/

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
		exit(EXIT_FAILURE);
	}

	// create socket

	if ((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		close(socketHandle);
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

/*void* readServerMessages(void* parameter){
	int socketConnection = args.socketConnection;
	while(1) {
		clientMsj msj = { };
		int result = readBlock(socketConnection, &msj, 60);
		if (result == 0){
			// error or eof
			cout << "EOF";
		}
		if (result < 0){
			cout << "ERROR";
			// error
		}
	}
}*/

/*void* readMsjs(void *param) {
	clientMsj clientMessage = { };
	while (1) {
		if (!messagesList.empty()) {
			clientMessage = messagesList.front();
			cout << clientMessage.id << endl;
			cout << clientMessage.value << endl;
			cout << clientMessage.type << endl;
			messagesList.pop_front();
		}
	}
	return 0;
}*/

void printMenu(list<clientMsj*> listaMensajes){
	cout<<"1. Conectar"<<endl;
	cout<<"2. Desconectar"<<endl;
	cout<<"3. Salir "<<endl;
	int i=0;
	list<clientMsj*>::iterator iterador;
	for (iterador = listaMensajes.begin(); iterador != listaMensajes.end(); iterador++){
		cout<< i+4 <<". Enviar mensaje "<<(*iterador)->id<<endl;
		i++;
	}
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

void cargarMensajes(list<clientMsj*> &listaMensajes, XmlParser parser){
	int cantidadMensajes = parser.cantidadMensajes();
	int contador;
	for(contador = 0; contador<cantidadMensajes;contador++){
		clientMsj* mensaje = (clientMsj*)malloc(sizeof(clientMsj));
		parser.obtenerMensaje(*mensaje, contador);
		listaMensajes.push_back(mensaje);
	}
}

/*void initThreadForReadingFromSocket(int socketConnection) {
	thread_args args;
	args.socketConnection = socketConnection;
	int error = pthread_create(&(clientThreadID[3]), NULL, &readServerMessages, &args);
	if (error != 0)
			printf("\ncan't create thread :[%s]", strerror(error));
}

void initThreadForReadingMessages(){
	int error = pthread_create(&(clientThreadID[2]), NULL, &readMsjs, NULL);
	if (error != 0)
		printf("\ncan't create thread :[%s]", strerror(error));
}*/

int checkPuerto(int puerto, ofstream* archivoErrores){
	if(puerto<1025 || puerto>65536){
		*archivoErrores<<"El puerto "<<puerto<<" no es válido."<<endl;
		return -1;
	}
	return 0;
}

int checkIp(string ip, ofstream* archivoErrores){
	if(ip.size()<7 || ip.size()>14){
		*archivoErrores<<"La ip "<<ip<<" no es válida."<<endl;
		return -1;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	//if(argc!=2){
	//	cout<<"Falta escribir el nombre del archvo!"<<endl;
	//	return -1;
	//}
	CargadorXML cargador;

	cargador.cargarServidor("clienteTest.txt");
	//cargador.cargarServidor(argv[1]);
	ofstream erroresXml; //Log de errores de mala escritura del XML.
	ofstream erroresConexion;
	erroresConexion.open("ErroresConexion",ios_base::app);
	ofstream erroresDatos;
	erroresDatos.open("erroresEnDatos",ios_base::app);
	XmlParser parser(cargador.getDocumento(), &erroresXml);
	string ip;
	int puerto;
	parser.obtenerIp(ip);
	parser.obtenerPuertoCl(puerto);
	//Chequeo si los datos estan bien. Si estan mal, cargo un xml por defecto.
	if(checkIp(ip, &erroresDatos)<0 || checkPuerto(puerto, &erroresDatos)<0){
		cargador.cargarServidor("clienteTest");
		XmlParser parser(cargador.getDocumento(), &erroresXml);
		parser.obtenerIp(ip);
		parser.obtenerPuertoCl(puerto);
	}
	cargarMensajes(messagesList, parser);
	int destinationSocket;
	//initThreadForReadingFromSocket(destinationSocket);
	//initThreadForReadingMessages();
	printMenu(messagesList);
	unsigned int opcion;
	bool fin = false;
	while (!fin){
		cin>>opcion;
		clientMsj disconnection, response;
		switch(opcion){
			case 1:
				destinationSocket = initializeClient(ip, puerto, &erroresConexion);
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
					parser.obtenerMensaje(mensaje, opcion - 4);
					int msjLength = sizeof(mensaje);
					write_socket(destinationSocket, &mensaje, msjLength, &erroresConexion);
					readBlock(destinationSocket, &response, sizeof(response),  &erroresConexion);
					cout << response.value << endl;
				}
				printMenu(messagesList);
		}

	}
	cout << "EXIT_SUCCESS"<<endl;
	return EXIT_SUCCESS;
}
