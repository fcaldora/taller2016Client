#ifndef WINDOW_H_
#define WINDOW_H_
#include <SDL2/SDL.h>
#include <string>
#include <iostream>

using namespace std;

class Window {
public:
	Window(string title, int height, int width);
	void paint();
	SDL_Renderer* getRenderer();
	virtual ~Window();

	int getHeight() const {
		return height;
	}

	void setHeight(int height) {
		this->height = height;
	}

	const string& getTitle() const {
		return title;
	}

	void setTitle(const string& title) {
		this->title = title;
	}

	int getWidth() const {
		return width;
	}

	void setWidth(int width) {
		this->width = width;
	}
	SDL_Window* window;
	SDL_Renderer* renderer;
private:

	int height;
	int width;
	string title;
};

#endif /* WINDOW_H_ */
