XmlLibrary = tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp

all:
	g++ -Wall XMLLoader.cpp -c
	g++ -Wall XmlParser.cpp -c
	g++ -Wall LogWriter.cpp -c

	g++ -Wall taller2016Client.cpp XMLLoader.o LogWriter.o XmlParser.o $(XmlLibrary) -o client -lpthread
