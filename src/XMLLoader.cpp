#include "XMLLoader.h"

#include <iostream>
#include <sstream>
#include <arpa/inet.h>
#include "Constants.h"

XMLLoader::XMLLoader(LogWriter *logWriter) {
	this->logWriter = logWriter;
}

bool XMLLoader::clientXMLIsValid(const char *fileName){
	TiXmlDocument xmlFile(fileName);

	if(!xmlFile.LoadFile()) {
		this->logWriter->writeNotFoundFileErrorForFileName(fileName);
		xmlFile.Clear();
		return false;
	}

	if (!clientXMLHasValidElements(xmlFile) || !clientXMLHasValidValues(xmlFile) || !clientXMLHasValidMessagesID()) {
		xmlFile.Clear();
		return false;
	}

	xmlFile.Clear();
	return true;
}

bool XMLLoader::clientXMLHasValidElements(TiXmlDocument xmlFile) {
	TiXmlElement *client = xmlFile.FirstChildElement(kClientTag);
	if (client == NULL){
		this->logWriter->writeNotFoundElementInXML(kClientTag);
		return false;
	}

	TiXmlElement *connection = client->FirstChildElement(kConnectionTag);
	if (connection == NULL) {
		this->logWriter->writeNotFoundElementInXML(kConnectionTag);
		return false;
	} else {
		TiXmlElement *clientIP = connection->FirstChildElement(kIPTag);
		if (clientIP == NULL){
			this->logWriter->writeNotFoundElementInXML(kIPTag);
			return false;
		}

		TiXmlElement *clientPort = clientIP->NextSiblingElement(kPortTag);
		if (clientPort == NULL){
			this->logWriter->writeNotFoundElementInXML(kPortTag);
			return false;
		}
	}

	TiXmlElement *messages = connection->NextSiblingElement(kMessagesTag);
	if (messages == NULL) {
		this->logWriter->writeNotFoundElementInXML(kMessagesTag);
		return false;
	}

	TiXmlElement *firstMessage = messages->FirstChildElement(kMessageTag);
	if (firstMessage == NULL){
		this->logWriter->writeNotFoundElementInXML(kMessageTag);
		return false;
	}

	TiXmlElement *firstMessageID = firstMessage->FirstChildElement(kMessageIDTag);
	if (firstMessageID == NULL) {
		this->logWriter->writeNotFoundElementInXML(kMessageIDTag);
		return false;
	}

	TiXmlElement *firstMessageType = firstMessageID->NextSiblingElement(kMessageTypeTag);
	if (firstMessageType == NULL) {
		this->logWriter->writeNotFoundElementInXML(kMessageTypeTag);
		return false;
	}

	TiXmlElement *firstMessageValue = firstMessageType->NextSiblingElement(kMessageValueTag);
	if (firstMessageValue == NULL) {
		this->logWriter->writeNotFoundElementInXML(kMessageValueTag);
		return false;
	}

	for(TiXmlElement *message = firstMessage->NextSiblingElement(kMessageTag); message != NULL; message = message->NextSiblingElement(kMessageTag)) {
		TiXmlElement *firstMessageID = firstMessage->FirstChildElement(kMessageIDTag);
		if (firstMessageID == NULL) {
			this->logWriter->writeNotFoundElementInXML(kMessageIDTag);
			return false;
		}

		TiXmlElement *firstMessageType = firstMessageID->NextSiblingElement(kMessageTypeTag);
		if (firstMessageType == NULL) {
			this->logWriter->writeNotFoundElementInXML(kMessageTypeTag);
			return false;
		}

		TiXmlElement *firstMessageValue = firstMessageType->NextSiblingElement(kMessageValueTag);
		if (firstMessageValue == NULL) {
			this->logWriter->writeNotFoundElementInXML(kMessageValueTag);
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
		this->logWriter->writeInvalidValueForElementInXML(kIPTag);
		return false;
	}

	const char *clientPort = connection->FirstChildElement(kIPTag)->NextSiblingElement(kPortTag)->GetText();
	std::stringstream portStrValue;
	portStrValue << clientPort;
	unsigned int portIntValue;
	portStrValue >> portIntValue;
	if (portIntValue <= 0 || portIntValue > kMaxNumberOfValidPort) {
		this->logWriter->writeInvalidValueForElementInXML(kPortTag);
		return false;
	}

	TiXmlElement *firstMessageElement = xmlFile.FirstChildElement(kClientTag)->FirstChildElement(kConnectionTag)->NextSiblingElement(kMessagesTag)->FirstChildElement(kMessageTag);
	for(TiXmlElement *message = firstMessageElement; message != NULL; message = message->NextSiblingElement(kMessageTag)) {
		const char *messageID = message->FirstChildElement(kMessageIDTag)->GetText();
		this->messageIDList.push_back(messageID);

		string messageType = stringFromChar(message->FirstChildElement(kMessageTypeTag)->GetText());
		if ((messageType.compare(kMessageTypeInt) != 0) && (messageType.compare(kMessageTypeString) != 0) && (messageType.compare(kMessageTypeChar) != 0) && (messageType.compare(kMessageTypeDouble) != 0)) {
			this->logWriter->writeInvalidValueForElementInXML(kMessageTypeTag);
			return false;
		}
	}

	return true;
}

bool XMLLoader::clientXMLHasValidMessagesID() {
	for (unsigned int i = 0 ; i < this->messageIDList.size() ; i ++) {
		for (unsigned int j = 0 ; j < this->messageIDList.size() ; j++) {
			if (i != j && (strcmp(this->messageIDList[i], this->messageIDList[j]) == 0)) {
				this->logWriter->writeMessagesIDAreDuplicated();
				return false;
			}
		}
	}

	return true;
}

string XMLLoader::stringFromChar(const char *value) {
	stringstream firstMessageTypeStreamString;
	firstMessageTypeStreamString << value;
	return firstMessageTypeStreamString.str();
}

XMLLoader::~XMLLoader() {

}
