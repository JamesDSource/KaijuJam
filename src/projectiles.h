#ifndef PROJECTILES__
#define PROJECTILES__

#include<SDL2/SDL.h>
#include<stdint.h>
#include"vec2.h"

typedef enum {
	PROJ_TYPE_PLAYER,
	PROJ_TYPE_ENEMY,
	PROJ_TYPE_COUNT
}ProjectileTypes;

typedef struct {
	uint32_t allocated;
	uint32_t count;

	ProjectileTypes* types;
	Vec2* positions;
	Vec2* velocities;
}Projectiles;

extern SDL_Texture* PROJECTILE_TEXTURES[PROJ_TYPE_COUNT];
extern float PROJECTILE_COL_RADIUS[PROJ_TYPE_COUNT];

void proj_init();
void proj_free();

Projectiles* proj_create();
void proj_destroy(Projectiles* proj);

#endif
