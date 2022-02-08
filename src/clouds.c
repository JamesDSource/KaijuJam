#include<stdio.h>
#include<SDL2/SDL.h>
#include <stdlib.h>
#include"clouds.h"
#include"draw.h"

const float CLOUD_OFFSCREEN_BUFFER = 80;
SDL_Texture* CLOUD_TEXTURES[CLOUD_TEX_COUNT];

void cloud_textures_init() {
	CLOUD_TEXTURES[CLOUD_TEX1] = load_texture("res/Cloud1.png");
	CLOUD_TEXTURES[CLOUD_TEX2] = load_texture("res/Cloud2.png");
	CLOUD_TEXTURES[CLOUD_TEX3] = load_texture("res/Cloud3.png");
	CLOUD_TEXTURES[CLOUD_TEX4] = load_texture("res/Cloud4.png");
	CLOUD_TEXTURES[CLOUD_TEX5] = load_texture("res/Cloud5.png");
}

void cloud_textures_free() {
	for(uint32_t i = 0; i < CLOUD_TEX_COUNT; ++i) {
		SDL_DestroyTexture(CLOUD_TEXTURES[i]);
	}
}

Cloud* clouds_init() {
	Cloud* cloud;
	cloud = malloc(sizeof(*cloud)*CLOUD_COUNT);
	for(uint32_t i = 0; i < CLOUD_COUNT; ++i) {
		cloud[i].texture_index = (((double)rand())/RAND_MAX)*CLOUD_TEX_COUNT;
		cloud[i].position = (Vec2){
			(((double)rand())/RAND_MAX)*(LEVEL_WIDTH),
			((double)rand())/RAND_MAX*(LEVEL_HEIGHT - CLOUD_MIN_HEIGHT)
		};
	}

	return cloud;
}

void clouds_free(Cloud* clouds) {
	free(clouds);
}

void clouds_draw(Cloud* clouds) {
	for(uint32_t i = 0; i < CLOUD_COUNT; ++i) {
		clouds[i].position.x -= 0.2;
		if(clouds[i].position.x < -CLOUD_OFFSCREEN_BUFFER) {
			clouds[i].position.x = LEVEL_WIDTH + CLOUD_OFFSCREEN_BUFFER;
			clouds[i].position = (Vec2){
				-CLOUD_OFFSCREEN_BUFFER,
				(((double)rand())/RAND_MAX)*(LEVEL_HEIGHT - CLOUD_MIN_HEIGHT)
			};
		}
		draw_texture(CLOUD_TEXTURES[clouds[i].texture_index], clouds[i].position.x - camera_x, clouds[i].position.y - camera_y, "cc", 0, SDL_FLIP_NONE);
	}
}
