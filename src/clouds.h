#ifndef CLOUDS__
#define CLOUDS__

#include<stdint.h>
#include"vec2.h"

#define CLOUD_COUNT 20
#define CLOUD_MIN_HEIGHT 168

typedef enum {
	CLOUD_TEX1,
	CLOUD_TEX2,
	CLOUD_TEX3,
	CLOUD_TEX4,
	CLOUD_TEX5,
	CLOUD_TEX_COUNT
}CloudTexture;

typedef struct {
	Vec2 position;
	uint32_t texture_index;
}Cloud;

void cloud_textures_init();
void cloud_textures_free();
Cloud* clouds_init();
void clouds_free(Cloud* clouds);
void clouds_update(Cloud* clouds);

#endif
