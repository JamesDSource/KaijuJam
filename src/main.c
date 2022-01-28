#include <SDL2/SDL_render.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include<emscripten.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

const uint32_t SCREEN_RES_W = 640;
const uint32_t SCREEN_RES_H = 480;

SDL_Window* window;
SDL_Renderer* renderer;

bool isRunning = true;
bool inMenu = true;

typedef enum {
	MENU_ITEM_LEVEL_ONE,
	MENU_ITEM_LEVEL_TWO,
	MENU_ITEM_LEVEL_THREE,
	MENU_ITEM_COUNT
}MenuItems;

const char* MENU_TEXT[MENU_ITEM_COUNT] = {
	"Level 1",
	"Level 2",
	"Level 3"
};

typedef struct {
	// Menu
	uint32_t menu_item_selected;
}Context;

void loop(void* arg) {
	Context* context = (Context*)arg;

	// Drawing the background
	SDL_SetRenderDrawColor(renderer, 20, 175, 160, 255);
	SDL_RenderClear(renderer);
	if(inMenu) {
		for(uint32_t i = 0; i < MENU_ITEM_COUNT; ++i) {
			//TTF_RenderText_Solid(NULL, MENU_TEXT[i], (SDL_Color){100, 100, 100, 255});
			//SDL_RenderCopy(renderer, NULL, NULL, NULL);
		}
	}

	SDL_RenderPresent(renderer);
	if(!isRunning) {
		emscripten_cancel_main_loop();
	}
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();	

	window = SDL_CreateWindow(
			"Attack of the Skyj***s", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_RES_W, SCREEN_RES_H, 
			SDL_WINDOW_SHOWN
	);
	renderer = SDL_CreateRenderer(window, -1, 0);

	Context context = {
		.menu_item_selected = 0
	};
	emscripten_set_main_loop_arg(loop, &context, 60, 1);
	
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
