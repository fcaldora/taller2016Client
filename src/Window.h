#ifndef WINDOW_H_
#define WINDOW_H_
#include <SDL2/SDL.h>
#include <string>

using namespace std;

class Window {
public:
	Window(string title, int height, int width);
	void paint();
	SDL_Renderer* getRenderer();
	virtual ~Window();

private:
	SDL_Window* window;
	int height;
	int width;
	string title;
	SDL_Renderer* renderer;
};

#endif /* WINDOW_H_ */
