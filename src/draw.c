#include"draw.h"

SDL_Texture* load_texture(const char* path) {
	SDL_Surface* surface = IMG_Load(path);
	assert(surface != NULL);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
	assert(tex != NULL);
	SDL_FreeSurface(surface);
	return tex;
}

void draw_texture(SDL_Texture* texture, int x, int y, char align[2]) {
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
		case 'd':
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

	SDL_RenderCopy(renderer, texture, NULL, &texture_rect);
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
