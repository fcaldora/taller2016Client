XmlLibrary = tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp

all:
	g++ -Wall XMLLoader.cpp -c -std=c++11
	g++ -Wall XmlParser.cpp -c -std=c++11
	g++ -Wall LogWriter.cpp -c -std=c++11
	g++ -Wall Window.cpp -c -lSDL2 -std=c++11
	g++ -Wall Messenger.cpp -c
	g++ -Wall Avion.cpp -c -lSDL2 -lSDL2_image -Wl,-rpath=/usr/local/lib -std=c++11
	g++ -Wall Background.cpp -c -lSDL2 -lSDL2_image -Wl,-rpath=/usr/local/lib -std=c++11
	g++ -Wall Client.cpp -c -std=c++11 -pthread
	g++ -Wall Object.cpp -c -std=c++11

	g++ -Wall -g taller2016Client.cpp XMLLoader.o LogWriter.o XmlParser.o Window.o Avion.o Messenger.o Background.o Client.o Object.o $(XmlLibrary) -o client -lSDL2  -pthread -lSDL2_image -Wl,-rpath=/usr/local/lib -std=c++11
																	
