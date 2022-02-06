#ifndef PLANE__
#define PLANE__

#include<stdint.h>
#include"vec2.h"

typedef enum {
	PLANE_STATUS_NOTHRUST = 1,
	PLANE_STATUS_DEAD = 2
}PlaneStatusFlags;

typedef enum {
	PLANE_TYPE_PLAYER
}PlaneTypes;

typedef struct {
	float accel;
	float turn_accel;
	float max_speed;
	float max_turn;
	float dir;
	float turn;
	float speed;
	Vec2 momentum;
}PlaneVel;

typedef struct {
	uint32_t allocated;
	uint32_t count;
	uint32_t* ids;
	Vec2* positions;
	PlaneVel* velocities;
	Vec2* targets;
	unsigned char* flags;
	PlaneTypes* types;

	// uint32_t projectiles_allocated;
	// uint32_t projectiles_count;
	// Vec2* projectile_positions;
	// unsigned char* projectile_flags;
}Planes;

extern const float GRAVITY;

Planes* planes_init();
void planes_free(Planes* planes);
uint32_t plane_add(Planes* planes, uint32_t* index);
void plane_remove(Planes* planes, uint32_t id);
uint32_t	plane_get(Planes* planes, uint32_t id); 
void planes_move(Planes* planes);

#endif
