#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include<emscripten.h>
#include<SDL2/SDL.h>

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;

	bool isRunning;
}App;

void loop(void* arg) {
	App app = *(App*)arg;
	SDL_RenderClear(app.renderer);
	SDL_Rect dest = {.x = 0, .y = 100, .w = 200, .h = 200};
	SDL_RenderPresent(app.renderer);

	if(!app.isRunning) {
		emscripten_cancel_main_loop();
	}
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);

	App app = {
		.isRunning = true
	};
	app.window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	app.renderer = SDL_CreateRenderer(app.window, -1, 0);

	emscripten_set_main_loop_arg(loop, &app, 60, 1);
	
	SDL_DestroyWindow(app.window);
	SDL_Quit();
	return 0;
}
