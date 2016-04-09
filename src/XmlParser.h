#include <string>
#include <iostream>
#include <fstream>
#include "tinyxml.h"

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

using namespace std;

#include "Constants.h"

class XmlParser {
public:
	XmlParser(const char* fileName);
	virtual ~XmlParser();

	int getNumberOfMessages();
	int getMessage(clientMsj &mensaje, int nroMensaje);
	int getServerPort();
	int cantidadMensajes();
	string getServerIP();
	LogLevelType getLogLevel();

private:
	TiXmlDocument doc;
};

#endif /* XMLPARSER_H_ */
