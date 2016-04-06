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
	bool clientXMLIsValid(const char* fileName);
	virtual ~XMLLoader();

private:
	bool clientXMLHasValidValues(TiXmlDocument xmlFile);
	bool clientXMLHasValidElements(TiXmlDocument xmlFile);
};

#endif /* CARGADORXML_H_ */
