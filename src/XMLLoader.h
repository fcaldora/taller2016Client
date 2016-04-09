#include "tinyxml.h"
#include <string>
#include <iostream>
#include <fstream>

#ifndef CARGADORXML_H_
#define CARGADORXML_H_

#include "LogWriter.h"

using namespace std;

class XMLLoader {
public:
	XMLLoader(LogWriter *logWriter);
	bool clientXMLIsValid(const char* fileName);
	virtual ~XMLLoader();

private:
	LogWriter *logWriter;
	vector<const char*> messageIDList;
	bool clientXMLHasValidValues(TiXmlDocument xmlFile);
	bool clientXMLHasValidElements(TiXmlDocument xmlFile);
	bool clientXMLHasValidMessagesID();
	string stringFromChar(const char *value);
};

#endif /* CARGADORXML_H_ */
