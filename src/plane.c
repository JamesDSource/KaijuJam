#include<stdlib.h>
#include<stdio.h>

#include"plane.h"

const float GRAVITY = 0.1;

Planes* planes_init() {
	Planes* planes = malloc(sizeof(Planes));
	const uint32_t alloc = 3;
	planes->allocated = alloc;
	planes->count = 0;

	planes->ids = malloc(sizeof(uint32_t)*alloc);
	planes->positions = malloc(sizeof(Vec2)*alloc);
	planes->velocities = malloc(sizeof(Vec2)*alloc);
	planes->flags = malloc(sizeof(unsigned char)*alloc);
	planes->types = malloc(sizeof(PlaneTypes)*alloc);
	return planes;
}

void planes_free(Planes* planes) {
	free(planes->ids);
	free(planes->positions);
	free(planes->velocities);
	free(planes->flags);
	free(planes->types);
	free(planes);
}

uint32_t plane_add(Planes* planes, uint32_t* index) {
	static uint32_t next_id = 0;
	uint32_t i = planes->count++;
	if(planes->count < planes->allocated) {
		const uint32_t alloc = planes->allocated *= 2;
		planes->ids = realloc(planes->ids, sizeof(uint32_t)*alloc);
		planes->positions = realloc(planes->positions, sizeof(Vec2)*alloc);
		planes->velocities = realloc(planes->velocities, sizeof(PlaneVel)*alloc);
		planes->flags = realloc(planes->flags, sizeof(unsigned char)*alloc);
		planes->types = realloc(planes->types, sizeof(PlaneTypes)*alloc);
	}
	
	planes->ids[i] = next_id;
	planes->positions[i] = (Vec2){0, 0};
	planes->velocities[i] = (PlaneVel){0, 0, 0, (Vec2){0, 0}};
	planes->flags[i] = 0;
	
	if(index != NULL) {
		*index = i;
	}
	return next_id++;
}

void plane_remove(Planes* planes, uint32_t id) {
	uint32_t last_index = --planes->count;
	for(int32_t i = planes->count - 2; i >= 0; --i) { // Starting from the second to last
		if(planes->ids[i] == id) {
			planes->ids[i] = planes->ids[last_index];
			planes->positions[i] = planes->positions[last_index];
			planes->velocities[i] = planes->velocities[last_index];
			planes->flags[i] = planes->flags[last_index];
		}
	}
}

uint32_t	plane_get(Planes* planes, uint32_t id) {
	for(uint32_t i = 0; i < planes->count; ++i) {
		if(planes->ids[i] == id) {
			return i;
		}
	}
	printf("ERROR in plane_get: Plane with id %u not found\n", id);
	exit(0);
}

void planes_move(Planes* planes) {
	for(uint32_t i = 0; i < planes->count; ++i) {
		if((planes->flags[i] & (PLANE_STATUS_DEAD | PLANE_STATUS_NOTHRUST)) != 0) {
			planes->positions[i].x += planes->velocities[i].momentum.x;
			planes->positions[i].y += (planes->velocities[i].momentum.y += GRAVITY);
		} else {
		}
	}
}
