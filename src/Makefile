XmlLibrary = tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp

all:
	g++ -Wall XMLLoader.cpp -c
	g++ -Wall XmlParser.cpp -c
	g++ -Wall LogWriter.cpp -c
	g++ -Wall Window.cpp -c -lSDL2
	g++ -Wall Avion.cpp -c -lSDL2 -lSDL2_image -Wl,-rpath=/usr/local/lib
	g++ -Wall Background.cpp -c -lSDL2 -lSDL2_image -Wl,-rpath=/usr/local/lib

	g++ -Wall taller2016Client.cpp XMLLoader.o LogWriter.o XmlParser.o Window.o Avion.o Background.o $(XmlLibrary) -o client -lSDL2  -pthread -lSDL2_image -Wl,-rpath=/usr/local/lib
																	
