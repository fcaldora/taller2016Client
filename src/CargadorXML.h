#include "tinyxml.h"
#include <string>
#include <iostream>
#include <fstream>

#ifndef CARGADORXML_H_
#define CARGADORXML_H_

using namespace std;

class CargadorXML {
public:
	CargadorXML();
	void cargarServidor(string nombreArchivo);
	void cargarCliente(string nombreArchivo);
	TiXmlDocument* getDocumento();
	virtual ~CargadorXML();

private:
	TiXmlDocument archivo;
	ofstream archivoErrores;
};

#endif /* CARGADORXML_H_ */
