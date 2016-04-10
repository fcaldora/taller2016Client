/*
 * ErrorLogsWriter.cpp
 *
 *  Created on: Apr 5, 2016
 *      Author: luciano
 */

#include "LogWriter.h"

#define kErrorsLogName "LogFile.txt"

LogWriter::LogWriter() {
	this->logFile.open(kErrorsLogName);
	this->logLevel = LogLevelTypeOnlyErrors;
}

LogWriter::~LogWriter() {
	this->logFile.close();
}

void LogWriter::setLogLevel(LogLevelType logLevel) {
	this->logLevel = logLevel;
}

void LogWriter::writeLogInFile(string log) {
	cout << log << endl;
	this->logFile << log << endl;
}

void LogWriter::writeNotFoundElementInXML(string elementName){
	string error = "El elemento '" + elementName + "' no existe. Se usara un XML por defecto.";
	writeLogInFile(error);
}

void LogWriter::writeInvalidValueForElementInXML(string element){
	string error = "El valor del elemento '" + element + "' es invalido. Se usara un XML por defecto.";
	writeLogInFile(error);
}

void LogWriter::writeMessagesIDAreDuplicated() {
	string error = "Uno o mas mensajes tienen el mismo ID. Se usara un XML por defecto";
	writeLogInFile(error);
}

void LogWriter::writeNotFoundFileErrorForFileName(string fileName) {
	string error = "El archivo '" + fileName + "' no existe o es invalido. Se usara uno por defecto.";
	writeLogInFile(error);
}

void LogWriter::writeUserDidnotEnterFileName() {
	writeLogInFile("Falta escribir el nombre del archivo del cliente, se usara uno por defecto.");
}

void LogWriter::writeUserDidSelectOption(int optionSelected) {
	if(this->logLevel == LogLevelTypeOnlyErrors)
		return;

	stringstream converter;
	converter << optionSelected;
	string log = "El usuario eligio la opcion " + converter.str() + ".";
	writeLogInFile(log);
}

void LogWriter::writeUserHasConnectedSuccessfully() {
	if(this->logLevel == LogLevelTypeOnlyErrors)
			return;

	string log = "El usuario se conecto satisfactoriamente.";
	writeLogInFile(log);
}

void LogWriter::writeCannotConnectDueToServerFull() {
	string log = "Error: no se pudo conectar porque el servidor esta lleno";
	writeLogInFile(log);
}

void LogWriter::writeConnectionErrorDescription(string description) {
	string error = "Hubo un error mientras el usuario se conectaba: " + description + "";
	writeLogInFile(error);
}

void LogWriter::writeUserHasDisconnectSuccessfully() {
	if(this->logLevel == LogLevelTypeOnlyErrors)
		return;

	string log = "El usuario se desconecto satisfactoriamente.";
	writeLogInFile(log);
}

void LogWriter::writeErrorConnectionHasClosed() {
	string error = "Error: se cerro la conexion";
	writeLogInFile(error);
}

void LogWriter::writeErrorInReceivingMessageWithID(char *messageID) {
	char* str1 = "Error: no se pudo recibir el mensaje con ID:";
	char* str2 = messageID;

	char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
	strcpy(str3, str1);
	strcat(str3, str2);
	writeLogInFile(str3);
	free(str3);
}

void LogWriter::writeReceivedSuccessfullyMessage(clientMsj *message) {
	if(this->logLevel == LogLevelTypeOnlyErrors)
		return;

	char *log = commentForMessage("Se recibio satisfactoriamente el mensaje con ID:", message);
	writeLogInFile(log);
	free(log);
}

void LogWriter::writeErrorInSendingMessage(clientMsj *message) {
	char *error = commentForMessage("Error: no se pudo enviar el mensaje con Id:", message);

	writeLogInFile(error);
	free(error);
}

void LogWriter::writeMessageSentSuccessfully(clientMsj *message) {
	if(this->logLevel == LogLevelTypeOnlyErrors)
		return;

	char *log = commentForMessage("Se envio satisfactoriamente el mensaje con Id:", message);
	writeLogInFile(log);
	free(log);
}

char * LogWriter::commentForMessage(char *headerText, clientMsj *message) {
	char* str1 = headerText;
	char* messageID = message->id;
	char *str2 = " tipo:";
	char *messageType = message->type;
	char *str3 = " valor:";
	char *messageValue = message->value;

	char * str4 = (char *) malloc(1 + strlen(str1)+ strlen(messageID)+ strlen(str2)+ strlen(messageType)+ strlen(str3)+ strlen(messageValue));
	strcpy(str4, str1);
	strcat(str4, messageID);
	strcat(str4, str2);
	strcat(str4, messageType);
	strcat(str4, str3);
	strcat(str4, messageValue);
	return str4;
}

void LogWriter::writeUserDidTerminateApp() {
	if(this->logLevel == LogLevelTypeOnlyErrors)
		return;
	string log = "El usuario cerro la aplicacion";
	writeLogInFile(log);
}
