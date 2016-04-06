XmlLibrary = tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp

all:
	g++ -Wall CargadorXML.cpp -c
	g++ -Wall XmlParser.cpp -c
	g++ -Wall ErrorLogsWriter.cpp -c

	g++ -Wall taller2016Client.cpp CargadorXML.o ErrorLogsWriter.o XmlParser.o $(XmlLibrary) -o client -lpthread
