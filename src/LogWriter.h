/*
 * ErrorLogsWriter.h
 *
 *  Created on: Apr 5, 2016
 *      Author: luciano
 */

#ifndef ERRORLOGSWRITER_H_
#define ERRORLOGSWRITER_H_

#include "Constants.h"

// Log Level: 	1 Only Errors (Default)
// 				2 Everything

class LogWriter {
public:
	LogWriter();
	virtual ~LogWriter();

	void writeUserDidnotEnterFileName();
	void writeInvalidValueForElementInXML(string element);
	void writeNotFoundElementInXML(string elementName);
	void writeNotFoundFileErrorForFileName(string fileName);
	void setLogLevel(LogLevelType logLevel);
	void writeUserDidSelectOption(int optionSelected);
	void writeUserHasConnectedSuccessfully();
	void writeConnectionErrorDescription(string description);
	void writeUserHasDisconnectSuccessfully();
	void writeErrorConnectionHasClosed();
	void writeErrorInSendingMessage(clientMsj *message);
	void writeMessageSentSuccessfully(clientMsj *message);
	void writeErrorInReceivingMessageWithID(char *messageID);
	void writeReceivedSuccessfullyMessage(clientMsj *message);
	void writeUserDidTerminateApp();
	void writeMessagesIDAreDuplicated();
	void writeCannotConnectDueToServerFull();

private:
	LogLevelType logLevel;
	ofstream logFile;
	void writeLogInFile(string error);
	char* commentForMessage(char *headerText, clientMsj *message);
};

#endif /* ERRORLOGSWRITER_H_ */
