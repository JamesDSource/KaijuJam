#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include<emscripten.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

const uint32_t SCREEN_RES_W = 640;
const uint32_t SCREEN_RES_H = 480;
const uint32_t SCREEN_SIZE_W = 1920;
const uint32_t SCREEN_SIZE_H = 1080;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* application_surface;

TTF_Font* font_sml;
TTF_Font* font_med;
TTF_Font* font_lrg;

bool isRunning = true;
bool inMenu = true;

int camera_x = 0, camera_y = 0;

void draw_text(
		const char* text,
		char size, 
		int x, int y, 
		char align,
		SDL_Color* color
) {
	TTF_Font* font;
	int font_size;
	switch(size) {
		case 's':
			font = font_sml;
			font_size = 12;
		case 'l':
			font = font_lrg;
			font_size = 48;
			break;
		case 'm':
		default:
			font = font_med;
			font_size = 24;
			break;
	}


	SDL_Surface* txt_surface = TTF_RenderText_Solid(font, text, color == NULL ? (SDL_Color){200, 200, 100, 255} : *color);
	SDL_Texture* txt_texture = SDL_CreateTextureFromSurface(renderer, txt_surface); 
	SDL_FreeSurface(txt_surface);

	int width = font_size*strlen(text)/2;
	SDL_Rect txt_rect = {
		.x = x,
		.y = y,
		.w = width,
		.h = font_size
	};

	switch(align) {
		case 'r':
			txt_rect.x -= width;
			break;
		case 'c':
			txt_rect.x -= width/2;
			break;
		case 'l':
		default:
			break;
	}
	
	SDL_RenderCopy(renderer, txt_texture, NULL, &txt_rect);
	SDL_DestroyTexture(txt_texture);
}

void loop(void* arg) {
	// Drawing the background
	SDL_SetRenderDrawColor(renderer, 66, 148, 173, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, application_surface);
	SDL_RenderClear(renderer);

	// Drawing the menu
	if(inMenu) {
		draw_text("ATTACK OF THE SKYJ***S", 'l', SCREEN_RES_W/2, SCREEN_RES_H/4, 'c', NULL);
		draw_text("Click to Start", 'm', SCREEN_RES_W/2, SCREEN_RES_H/2, 'c', NULL);
	}

	SDL_SetRenderTarget(renderer, NULL);
	SDL_RenderCopy(renderer, application_surface, NULL, NULL);
	SDL_RenderPresent(renderer);
	if(!isRunning) {
		emscripten_cancel_main_loop();
	}
}

int main() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();	
	font_sml = TTF_OpenFont("res/FreePixel.ttf", 12);
	font_med = TTF_OpenFont("res/FreePixel.ttf", 24);
	font_lrg = TTF_OpenFont("res/FreePixel.ttf", 48);
  TTF_SetFontStyle(font_lrg, TTF_STYLE_BOLD);

	window = SDL_CreateWindow(
			"Attack of the Skyj***s", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_SIZE_W,
			SCREEN_SIZE_H, 
			SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);
	application_surface = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA32, 
		SDL_TEXTUREACCESS_TARGET, 
		SCREEN_RES_W, 
		SCREEN_RES_H);

	emscripten_set_main_loop_arg(loop, NULL, 60, 1);
	
	TTF_CloseFont(font_sml);
	TTF_CloseFont(font_med);
	TTF_CloseFont(font_lrg);
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(application_surface);
	SDL_Quit();
	return 0;
}
