/*
 * Window.cpp
 *
 *  Created on: 22 de abr. de 2016
 *      Author: gusta
 */

#include "Window.h"

Window::Window(string title, int height, int width) {
	if(SDL_Init(SDL_INIT_EVERYTHING) == 0){
		this->height = height;
		this->width = width;
		this->title = title;
		this->window =
				SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
		this->renderer = SDL_CreateRenderer( this->window, -1, SDL_RENDERER_SOFTWARE);
		SDL_SetRenderDrawColor(this->renderer, 0,0,0,255);
	}else{
		cout << "SDL no inicializo correctamente" << endl;
	}
}
void Window::paint(){
	SDL_RenderPresent(this->renderer);
}
SDL_Renderer* Window::getRenderer(){
	return this->renderer;
}

void Window::paintAll(list<Object> objects){
	list<Object>::iterator iterador = objects.begin(); //El primer elemento es el escenario.
	(*iterador).paint(renderer, (*iterador).getPosX(), (*iterador).getPosY());
	(*iterador).paint(renderer, (*iterador).getPosX(), (*iterador).getPosY() - (*iterador).getHeight());
	iterador++;
	for (; iterador != objects.end(); iterador++){
		(*iterador).paint(renderer, (*iterador).getPosX(), (*iterador).getPosY());
	}
}

void Window::clear(){
	SDL_RenderClear(renderer);
}

Window::~Window() {
	SDL_DestroyWindow(window);
	SDL_RenderClear(this->renderer);
	SDL_DestroyRenderer(this->renderer);
}

