#ifndef MAIN__
#define MAIN__

#include<SDL2/SDL_render.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include<emscripten.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>

extern const uint32_t SCREEN_RES_W;
extern const uint32_t SCREEN_RES_H;
extern const uint32_t SCREEN_SIZE_W;
extern const uint32_t SCREEN_SIZE_H;

extern const uint32_t LEVEL_HEIGHT;
extern const uint32_t LEVEL_WIDTH;
extern const uint32_t OCEAN_HEIGHT;

extern const float GRAVITY;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* application_surface;

extern TTF_Font* font_sml;
extern TTF_Font* font_med;
extern TTF_Font* font_lrg;

extern int mouse_screen_x;
extern int mouse_screen_y;

#endif
