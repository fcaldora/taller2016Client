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

	TiXmlElement *firstMessageType = firstMessageID->NextSiblingElement(kMesssageTypeTag);
	if (firstMessageType == NULL) {
		this->logWriter->writeNotFoundElementInXML(kMesssageTypeTag);
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

		TiXmlElement *firstMessageType = firstMessageID->NextSiblingElement(kMesssageTypeTag);
		if (firstMessageType == NULL) {
			this->logWriter->writeNotFoundElementInXML(kMesssageTypeTag);
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
		this->logWriter->writeInvalidadValueForElementInXML(kIPTag);
		return false;
	}

	const char *clientPort = connection->FirstChildElement(kIPTag)->NextSiblingElement(kPortTag)->GetText();
	std::stringstream portStrValue;
	portStrValue << clientPort;
	unsigned int portIntValue;
	portStrValue >> portIntValue;
	if (portIntValue <= 0 || portIntValue > kMaxNumberOfValidPort) {
		this->logWriter->writeInvalidadValueForElementInXML(kPortTag);
		return false;
	}

	TiXmlElement *firstMessage = xmlFile.FirstChildElement(kClientTag)->FirstChildElement(kConnectionTag)->NextSiblingElement(kMessagesTag)->FirstChildElement(kMessageTag);
	for(TiXmlElement *message = firstMessage; message != NULL; message = message->NextSiblingElement(kMessageTag)) {
		const char *firstMessageType = message->FirstChildElement(kMesssageTypeTag)->GetText();
		stringstream firstMessageTypeStreamString;
		firstMessageTypeStreamString << firstMessageType;
		string firstMessageTypeString = firstMessageTypeStreamString.str();
		if ((firstMessageTypeString.compare(kMessageTypeInt) != 0) && (firstMessageTypeString.compare(kMessageTypeString) != 0) && (firstMessageTypeString.compare(kMessageTypeChar) != 0) && (firstMessageTypeString.compare(kMessageTypeDouble) != 0)) {
			this->logWriter->writeInvalidadValueForElementInXML(kMesssageTypeTag);
			return false;
		}
	}

	return true;
}

XMLLoader::~XMLLoader() {

}
