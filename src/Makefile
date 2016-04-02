XmlLibrary = tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp

all:
	g++ -Wall CargadorXML.cpp -c
	g++ -Wall XmlParser.cpp -c

	g++ -Wall taller2016Client.cpp CargadorXML.o XmlParser.o $(XmlLibrary) -o client -lpthread
