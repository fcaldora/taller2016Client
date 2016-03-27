#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <string.h>
#define MAXHOSTNAME 256
using namespace std;

struct clientMsj {
	int id;
	char type[20];
	char value[20];
};

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

int main() {

	//genero un mensaje de prueba para enviar al server
	clientMsj testMsj = { };
	testMsj.id = 10;
	strcpy(testMsj.type, "mensaje de prueba");
	strcpy(testMsj.value, "avanzar nave");

	int destinationSocket = initializeClient("127.0.0.1", 8080);
	write_socket(destinationSocket, &testMsj, 44);

}
