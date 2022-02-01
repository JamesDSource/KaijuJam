#ifndef DRAW__
#define DRAW__

#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_image.h>
#include<assert.h>

#include"main.h"

SDL_Texture* load_texture(const char* path);

void draw_texture(SDL_Texture* texture, int x, int y, char align[2]);
void draw_text(
		const char* text,
		TTF_Font* font,
		int x, int y, 
		char align[2],
		SDL_Color* color);

#endif