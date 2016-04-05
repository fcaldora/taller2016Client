#include "tinyxml.h"
#include <string>
#include <iostream>
#include <fstream>

#ifndef CARGADORXML_H_
#define CARGADORXML_H_

using namespace std;

class XMLLoader {
public:
	XMLLoader();
	void cargarServidor(string nombreArchivo);
	void cargarCliente(string nombreArchivo);
	TiXmlDocument* getDocumento();
	virtual ~XMLLoader();

private:
	TiXmlDocument xmlDocument;
	ofstream errorLogFile;
};

#endif /* CARGADORXML_H_ */
