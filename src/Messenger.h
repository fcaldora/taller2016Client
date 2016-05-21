/*
 * Messenger.h
 *
 *  Created on: 21 de may. de 2016
 *      Author: gusta
 */

#ifndef MESSENGER_H_
#define MESSENGER_H_
#include <sys/socket.h>
#include "Constants.h"
#include <unistd.h>
class Messenger {
public:
	Messenger();
	virtual ~Messenger();
	int readClientMsj(int socket, int bytesARecibir, clientMsj* mensaje);
	int readMensaje(int socket, int bytesARecibir, mensaje* mensaje);
	int readPreMsj(int socket, int bytesARecibir, actionMsj* mensaje);
	int readUpdateMsj(int socket, int bytesARecibir, updateMsj* mensaje);
	int readDeleteMsj(int socket, int bytesARecibir, deleteMsj* mensaje);
	int sendClientMsj(int socket, int bytesAEnviar, clientMsj* mensaje);
	int readActionMsj(int socket, int bytesARecibir, actionMsj* mensaje);
};

#endif /* MESSENGER_H_ */
