#include"draw.h"
#include <SDL2/SDL_render.h>

SDL_Texture* load_texture(const char* path) {
	SDL_Surface* surface = IMG_Load(path);
	assert(surface != NULL);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
	assert(tex != NULL);
	SDL_FreeSurface(surface);
	return tex;
}

SDL_Texture** load_texture_strip(const char* path, uint32_t segments) {
	SDL_Surface* surface = IMG_Load(path);
	assert(surface != NULL);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
	assert(tex != NULL);
	SDL_FreeSurface(surface);

	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	int seg_w = w/segments;

	SDL_Texture** textures;
	textures = malloc(sizeof(void*)*segments);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	for(uint32_t i = 0; i < segments; ++i) {
		textures[i] = SDL_CreateTexture(renderer, 
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET,
			seg_w,
			h);
		assert(textures[i] != NULL);
		SDL_Rect src_rect = {
			.x = i*seg_w,
			.y = 0,
			.w = seg_w,
			.h = h
		};
		SDL_SetTextureBlendMode(textures[i], SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(renderer, textures[i]);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, tex, &src_rect, NULL);
	}
	SDL_SetRenderTarget(renderer, application_surface);
	SDL_DestroyTexture(tex);

	return textures;
}

void free_texture_strip(SDL_Texture** strip, uint32_t segments) {
	for(uint32_t i = 0; i < segments; ++i) {
		SDL_DestroyTexture(strip[i]);
	}
	free(strip);
}

void draw_texture(SDL_Texture* texture, int x, int y, char align[2], float rotation, SDL_RendererFlip flip) {
	SDL_Rect texture_rect = {
		.x = x,
		.y = y
	};
	SDL_QueryTexture(texture, NULL, NULL, &texture_rect.w, &texture_rect.h);

	switch(align[0]) {
		case 't':
			texture_rect.y -= texture_rect.h;
			break;
		case 'c':
			texture_rect.y -= texture_rect.h/2;
			break;
		case 'b':
		default:
			break;
	}
	
	switch(align[1]) {
		case 'r':
			texture_rect.x -= texture_rect.w;
			break;
		case 'c':
			texture_rect.x -= texture_rect.w/2;
			break;
		case 'l':
		default:
			break;
	}

	SDL_RenderCopyEx(renderer, texture, NULL, &texture_rect, rotation, NULL, flip);
}

void draw_text(
		const char* text,
		TTF_Font* font,
		int x, int y, 
		char align[2],
		SDL_Color* color
) {
	SDL_Surface* txt_surface = TTF_RenderText_Solid(font, text, color == NULL ? (SDL_Color){200, 200, 100, 255} : *color);
	SDL_Texture* txt_texture = SDL_CreateTextureFromSurface(renderer, txt_surface); 
	SDL_FreeSurface(txt_surface);

	SDL_Rect txt_rect = {
		.x = x,
		.y = y,
	};
	SDL_QueryTexture(txt_texture, NULL, NULL, &txt_rect.w, &txt_rect.h);


	switch(align[0]) {
		case 't':
			txt_rect.y -= txt_rect.h;
			break;
		case 'c':
			txt_rect.y -= txt_rect.h/2;
			break;
		case 'd':
		default:
			break;
	}
	
	switch(align[1]) {
		case 'r':
			txt_rect.x -= txt_rect.w;
			break;
		case 'c':
			txt_rect.x -= txt_rect.w/2;
			break;
		case 'l':
		default:
			break;
	}
	
	SDL_RenderCopy(renderer, txt_texture, NULL, &txt_rect);
	SDL_DestroyTexture(txt_texture);
}
