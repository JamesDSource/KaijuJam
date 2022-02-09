#ifndef PROJECTILES__
#define PROJECTILES__

#include<SDL2/SDL.h>
#include<stdint.h>
#include"vec2.h"
#include"plane.h"

typedef enum {
	PROJ_TYPE_PLAYER,
	PROJ_TYPE_DRAGFLY_FIRE,
	PROJ_TYPE_COUNT
}ProjectileTypes;

typedef struct {
	uint32_t allocated;
	uint32_t count;

	ProjectileTypes* types;
	Vec2* positions;
	Vec2* velocities;
	uint32_t* life;
}Projectiles;

extern SDL_Texture* PROJECTILE_TEXTURES[PROJ_TYPE_COUNT];
extern float PROJECTILE_COL_RADIUS[PROJ_TYPE_COUNT];

void proj_init();
void proj_free();

Projectiles* proj_create();
void proj_destroy(Projectiles* proj);
void proj_add(Projectiles* proj, ProjectileTypes type, Vec2 pos, float angle_rad, float speed);
void proj_remove(Projectiles* proj, uint32_t index);
void proj_move(Projectiles* proj);
void proj_draw(Projectiles* proj);
void proj_check_collision(Projectiles* proj, Planes* planes);

#endif
