#include"projectiles.h"
#include"draw.h"

SDL_Texture* PROJECTILE_TEXTURES[PROJ_TYPE_COUNT] = {};
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
	const uint32_t alloc = 3;
	proj->allocated = alloc;
	proj->count = 0;

	proj->types = malloc(sizeof(*proj->types)*alloc);
	proj->positions = malloc(sizeof(*proj->positions)*alloc);
	proj->velocities = malloc(sizeof(*proj->velocities)*alloc);

	return proj;
}

void proj_destroy(Projectiles* proj) {
	free(proj->types);
	free(proj->positions);
	free(proj->velocities);
	free(proj);
}

void proj_add(Projectiles* proj, ProjectileTypes type, Vec2 pos, float angle_rad, float speed) {
	uint32_t index = proj->count++;
	if(proj->count > proj->allocated) {
		uint32_t alloc = proj->allocated *= 2;
		proj->types = realloc(proj->types, sizeof(*proj->types)*alloc);
		proj->positions = realloc(proj->positions, sizeof(*proj->positions)*alloc);
		proj->velocities = realloc(proj->velocities, sizeof(*proj->velocities)*alloc);
	}
	
	proj->types[index] = type;
	proj->positions[index] = pos;
	proj->velocities[index] = (Vec2){cos(angle_rad)*speed, sin(angle_rad)*speed};
}

void proj_remove(Projectiles* proj, uint32_t index) {
	uint32_t last_index = --proj->count;
	if(last_index == index) {
		return;
	} else {
		proj->types[index] = proj->types[last_index];
		proj->positions[index] = proj->positions[last_index];
		proj->velocities[index] = proj->velocities[last_index];
	}
}

void proj_move(Projectiles* proj) {
	for(uint32_t i = 0; i < proj->count; ++i) {
		proj->positions[i].x += proj->velocities[i].x;
		proj->positions[i].y += proj->velocities[i].y;
	}
}

void proj_draw(Projectiles* proj) {
	for(uint32_t i = 0; i < proj->count; ++i) {
		ProjectileTypes type = proj->types[i];
		Vec2 pos = proj->positions[i];
		draw_texture(PROJECTILE_TEXTURES[type], pos.x - camera_x, pos.y - camera_y, "cc", 0, SDL_FLIP_NONE);
	}
}
