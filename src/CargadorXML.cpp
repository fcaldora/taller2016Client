#include "CargadorXML.h"

#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include "Constants.h"

XMLLoader::XMLLoader() {

}

bool XMLLoader::clientXMLIsValid(const char *fileName){
	TiXmlDocument xmlFile(fileName);

	if(!xmlFile.LoadFile()) {
		cout << "NOT LOAD FILE" << endl;
		//this->errorLogWriter->writeNotFoundFileForNameError(fileName);
		xmlFile.Clear();
		return false;
	}

	if (!clientXMLHasValidElements(xmlFile) || !clientXMLHasValidValues(xmlFile)) {
		xmlFile.Clear();
		return false;
	}

	xmlFile.Clear();
	return true;
}

bool XMLLoader::clientXMLHasValidElements(TiXmlDocument xmlFile) {
	TiXmlElement *client = xmlFile.FirstChildElement(kClientTag);
	if (client == NULL){
		cout << "client NULL" << endl;
		//this->errorLogWriter->writeNotFoundElementInXML("Servidor");
		return false;
	}

	TiXmlElement *connection = client->FirstChildElement(kConnectionTag);
	if (connection == NULL) {
		cout << "connection NULL" << endl;
		//this->errorLogWriter->writeNotFoundElementInXML("CantidadMaximaClientes");
		return false;
	} else {
		TiXmlElement *clientIP = connection->FirstChildElement(kIPTag);
		if (clientIP == NULL){
			cout << "client IP NULL" << endl;
			return false;
		}

		TiXmlElement *clientPort = clientIP->NextSiblingElement(kPortTag);
		if (clientPort == NULL){
			cout << "client Port NULL" << endl;
			return false;
		}
	}

	TiXmlElement *messages = connection->NextSiblingElement(kMessagesTag);
	if (messages == NULL) {
		cout << "messages NULL" << endl;
		//this->errorLogWriter->writeNotFoundElementInXML("Puerto");
		return false;
	}

	TiXmlElement *firstMessage = messages->FirstChildElement(kMessageTag);
	if (firstMessage == NULL){
		cout << "first message NULL" << endl;

		return false;
	}

	TiXmlElement *firstMessageID = firstMessage->FirstChildElement(kMessageIDTag);
	if (firstMessageID == NULL) {
		cout << "first message ID NULL" << endl;

		return false;
	}

	TiXmlElement *firstMessageType = firstMessageID->NextSiblingElement(kMesssageTypeTag);
	if (firstMessageType == NULL) {
		cout << "first message type NULL" << endl;

		return false;
	}

	TiXmlElement *firstMessageValue = firstMessageType->NextSiblingElement(kMessageValueTag);
	if (firstMessageValue == NULL) {
		cout << "first message value NULL" << endl;

		return false;
	}

	for(TiXmlElement *message = firstMessage->NextSiblingElement(kMessageTag); message != NULL; message = message->NextSiblingElement(kMessageTag)) {
		TiXmlElement *firstMessageID = firstMessage->FirstChildElement(kMessageIDTag);
		if (firstMessageID == NULL) {
			cout << "first message ID NULL" << endl;

			return false;
		}

		TiXmlElement *firstMessageType = firstMessageID->NextSiblingElement(kMesssageTypeTag);
		if (firstMessageType == NULL) {
			cout << "first message type NULL" << endl;

			return false;
		}

		TiXmlElement *firstMessageValue = firstMessageType->NextSiblingElement(kMessageValueTag);
		if (firstMessageValue == NULL) {
			cout << "first message value NULL" << endl;

			return false;
		}
	}

	return true;
}

bool XMLLoader::clientXMLHasValidValues(TiXmlDocument xmlFile){
	TiXmlElement *connection = xmlFile.FirstChildElement(kClientTag)->FirstChildElement(kConnectionTag);
	const char* clientIP = connection->FirstChildElement(kIPTag)->GetText();
	struct sockaddr_in sa;
	int success = inet_pton(AF_INET, clientIP, &(sa.sin_addr));
	if (success != 1){
		cout << "invalid IP" << endl;
		return false;
	}

	const char *clientPort = connection->FirstChildElement(kIPTag)->NextSiblingElement(kPortTag)->GetText();
	std::stringstream portStrValue;
	portStrValue << clientPort;
	unsigned int portIntValue;
	portStrValue >> portIntValue;
	if (portIntValue <= 0 || portIntValue > kMaxNumberOfValidPort) {
		cout << "invalid Port" << endl;

		//this->errorLogWriter->writeValueErrorForElementInXML("Puerto");
		return false;
	}

	TiXmlElement *firstMessage = xmlFile.FirstChildElement(kClientTag)->FirstChildElement(kConnectionTag)->NextSiblingElement(kMessagesTag)->FirstChildElement(kMessageTag);
	for(TiXmlElement *message = firstMessage; message != NULL; message = message->NextSiblingElement(kMessageTag)) {
		const char *firstMessageType = message->FirstChildElement(kMesssageTypeTag)->GetText();
		stringstream firstMessageTypeStreamString;
		firstMessageTypeStreamString << firstMessageType;
		string firstMessageTypeString = firstMessageTypeStreamString.str();
		if ((firstMessageTypeString.compare(kMessageTypeInt) != 0) && (firstMessageTypeString.compare(kMessageTypeString) != 0) && (firstMessageTypeString.compare(kMessageTypeChar) != 0) && (firstMessageTypeString.compare(kMessageTypeDouble) != 0)) {
			cout << "invalid Message Type" << endl;
			return false;
		}
	}

	return true;
}

XMLLoader::~XMLLoader() {

}
