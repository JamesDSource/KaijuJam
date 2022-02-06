#include"projectiles.h"
#include"draw.h"

SDL_Texture* PROJECTILE_TEXTURES[PROJ_TYPE_COUNT] = {

};

float PROJECTILE_COL_RADIUS[PROJ_TYPE_COUNT] = {
	8, 8
};

void proj_init() {
	PROJECTILE_TEXTURES[PROJ_TYPE_PLAYER] = load_texture("res/Bullet.png");
	PROJECTILE_TEXTURES[PROJ_TYPE_ENEMY] = load_texture("res/EnemyBullet.png");
}

void proj_free() {
	for(uint32_t i = 0; i < PROJ_TYPE_COUNT; ++i) {
		SDL_DestroyTexture(PROJECTILE_TEXTURES[i]);
	}
}

Projectiles* proj_create() {
	Projectiles* proj;
	proj = malloc(sizeof(*proj));

	return proj;
}

void proj_destroy(Projectiles* proj) {
	free(proj);
}
