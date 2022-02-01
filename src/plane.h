#ifndef PLANE__
#define PLANE__

#include<stdint.h>

typedef enum {
	PLANE_STATUS_NOTHRUST = 1,
	PLANE_STATUS_PLAYER = 2,
	PLANE_STATUS_DEAD = 4,
}PlaneStatusFlags;

typedef struct {
	float x, y;
}Vec2;

typedef struct {
	uint32_t allocated;
	uint32_t count;
	uint32_t* ids;
	Vec2* positions;
	Vec2* velocities;
	unsigned char* flags;
}Planes;

extern const float GRAVITY;

Planes* planes_init();
void planes_free(Planes* planes);
uint32_t plane_add(Planes* planes, uint32_t* index);
void plane_remove(Planes* planes, uint32_t id);
uint32_t	plane_get(Planes* planes, uint32_t id); 

#endif
