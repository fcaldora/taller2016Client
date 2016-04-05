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
	XmlParser(TiXmlDocument* doc, ofstream* archivoErrores);
	virtual ~XmlParser();
	int cantidadMensajes();
	int obtenerMensaje(clientMsj &mensaje, int nroMensaje);
	void getServerPort(int &puerto);
	int getClientPort();
	string getIP();
	void getMaxNumberOfClients(int &maxClientes);
private:
	TiXmlDocument* doc;
	ofstream* archivoErrores;
};

#endif /* XMLPARSER_H_ */
