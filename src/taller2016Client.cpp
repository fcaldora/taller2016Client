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

#define MAXHOSTNAME 256
using namespace std;

list<clientMsj> messagesList;
pthread_t clientThreadID[3];

struct thread_args {
    int socketConnection;
}args;

int readBlock(int fd, void* buffer, int len) {
	int ret = 0;
	int count = 0;
	while (count < len) {
		ret = read(fd, buffer + count, 1);
		if (ret = 0) {
			return 0;
		}
		if (ret < 0){
			return -1;
		}
		count += 1;
	}
	return count;
}

int initializeClient(string destinationIp, int port) {
	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;
	int socketHandle;
	const char *remoteHost = destinationIp.c_str();
	int portNumber = port;

	bzero(&remoteSocketInfo, sizeof(sockaddr_in));  // Clear structure memory

	// Get system information

	if ((hPtr = gethostbyname(remoteHost)) == NULL) {
		cerr << "System DNS name resolution not configured properly." << endl;
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
		exit(EXIT_FAILURE);
	}

	return socketHandle;
}

int write_socket(int destinationSocket, void *buf, int len) {
	int currentsize = 0;
	int count = 0;
	while (currentsize < len) {
		count = write(destinationSocket, buf + currentsize, 1);
		if (count < 0)
			return -1;
		currentsize += 1;
	}
	return currentsize;
}

void* readServerMessages(void* parameter){
	int socketConnection = args.socketConnection;
	while(1) {
		clientMsj msj = { };
		int result = readBlock(socketConnection, &msj, 60);
		if (result = 0){
			// error or eof
			cout << "EOF";
		}
		if (result < 0){
			cout << "ERROR";
			// error
		}
	}
}

void* readMsjs(void *param) {
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
}

void initThreadForReadingFromSocket(int socketConnection) {
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
}

int main(int argc, char* argv[]) {
	if(argc!=2){
		cout<<"Falta escribir el nombre del archvo!"<<endl;
		return -1;
	}

	//genero un mensaje de prueba para enviar al server
	clientMsj testMsj = { };
	strcpy(testMsj.id, "10");
	strcpy(testMsj.type, "mensaje de prueba");
	strcpy(testMsj.value, "avanzar nave");
	CargadorXML cargador;

	//cargador.cargarServidor("clienteTest.txt");
	cargador.cargarServidor(argv[1]);

	XmlParser parser(cargador.getDocumento());
	string ip;
	int puerto;
	parser.obtenerIp(ip);
	parser.obtenerPuertoCl(puerto);

	int destinationSocket = initializeClient(ip, puerto);
	initThreadForReadingFromSocket(destinationSocket);
	initThreadForReadingMessages();
	write_socket(destinationSocket, &testMsj, 60);

	messagesList.push_back(testMsj);

	pthread_join(clientThreadID[2], NULL);
	pthread_join(clientThreadID[3], NULL);
	close(destinationSocket);

	cout << "EXIT_SUCCESS";
	return EXIT_SUCCESS;
}
