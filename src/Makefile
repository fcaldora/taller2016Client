XmlLibrary = tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp

all:
	g++ -Wall XMLLoader.cpp -c -std=c++11
	g++ -Wall XmlParser.cpp -c -std=c++11
	g++ -Wall LogWriter.cpp -c -std=c++11
	g++ -Wall Window.cpp -c -lSDL2 -std=c++11
	g++ -Wall MenuPresenter.cpp -c -lSDL2 -std=c++11
	g++ -Wall Avion.cpp -c -lSDL2 -lSDL2_image -Wl,-rpath=/usr/local/lib -std=c++11
	g++ -Wall Background.cpp -c -lSDL2 -lSDL2_image -Wl,-rpath=/usr/local/lib -std=c++11
	g++ -Wall Client.cpp -c -std=c++11 -pthread
	g++ -Wall Object.cpp -c -std=c++11
	g++ -Wall Score.cpp -c -std=c++11
	g++ -Wall StageInfo.cpp -c -std=c++11
	g++ -Wall ScoresManager.cpp -c -std=c++11

	g++ -Wall -g taller2016Client.cpp XMLLoader.o StageInfo.o Score.o LogWriter.o XmlParser.o MenuPresenter.o Window.o Avion.o Background.o Client.o Object.o ScoresManager.o $(XmlLibrary) -o client -lSDL2  -pthread -lSDL2_image -lSDL2_mixer -Wl,-rpath=/usr/local/lib -lSDL2_ttf -std=c++11
																	
