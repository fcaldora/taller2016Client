#include <string>
#include <iostream>
#include <fstream>
#include "tinyxml.h"

#define LONGCHAR 20
#define PUERTO_PREDEF 8080

struct clientMsj {
	char id[LONGCHAR];
	char type[LONGCHAR];
	char value[LONGCHAR];
};

#ifndef XMLPARSER_H_
#define XMLPARSER_H_

using namespace std;

class XmlParser {
public:
	XmlParser(const char* fileName, ofstream* archivoErrores);
	virtual ~XmlParser();
	int getNumberOfMessages();
	int getMessage(clientMsj &mensaje, int nroMensaje);
	int getServerPort();
	int cantidadMensajes();
	string getServerIP();

private:
	TiXmlDocument doc;
	ofstream* archivoErrores;
};

#endif /* XMLPARSER_H_ */
