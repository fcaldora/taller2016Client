/*
 * Client.h
 *
 *  Created on: Apr 25, 2016
 *      Author: facundo
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <thread>

using namespace std;

class Client {
public:
	Client();
	virtual ~Client();

	int getSocketConnection() const {
		return socketConnection;
	}

	void setSocketConnection(int socketConnection) {
		this->socketConnection = socketConnection;
	}

	thread threadSDL;
	thread threadListen;
	thread threadKeepAlive;

private:
	int socketConnection;

};

#endif /* CLIENT_H_ */
