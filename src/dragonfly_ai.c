#include<stdlib.h>
#include"dragonfly_ai.h"

DragonflyActors* dragonfly_actors_create() {
	DragonflyActors* actors;
	actors = malloc(sizeof(*actors));

	const uint32_t alloc = 3;
	actors->allocated = alloc;
	actors->count = 0;

	actors->plane_ids = malloc(sizeof(*actors->plane_ids)*alloc);
	actors->fireball_cooldowns = malloc(sizeof(*actors->fireball_cooldowns)*alloc);
	actors->current_frames = malloc(sizeof(*actors->current_frames)*alloc);
	return actors;
}

void dragonfly_actors_destroy(DragonflyActors* actors) {
	free(actors->fireball_cooldowns);
	free(actors->current_frames);
	free(actors);
}

void dragonfly_actors_add(DragonflyActors* actors, uint32_t plane_id) {
	uint32_t index = actors->count++;
	if(actors->count > actors->allocated) {
		uint32_t alloc = actors->allocated *= 2;
		actors->plane_ids = realloc(actors->plane_ids, sizeof(*actors->plane_ids)*alloc);
		actors->fireball_cooldowns = realloc(actors->fireball_cooldowns, sizeof(*actors->fireball_cooldowns)*alloc);
		actors->current_frames = realloc(actors->current_frames, sizeof(*actors->current_frames)*alloc);
	}

	actors->plane_ids[index] = plane_id;
	actors->fireball_cooldowns[index] = DF_FIREBALL_COOLDOWN;
	actors->current_frames[index] = 0;
}

void dragonfly_actors_remove(DragonflyActors* actors, uint32_t index) {
	uint32_t last_index = --actors->count;	
	if(last_index == index) {
		return;
	}

	actors->plane_ids[index] = actors->plane_ids[last_index];
	actors->fireball_cooldowns[index] = actors->fireball_cooldowns[last_index];
	actors->current_frames[index] = actors->current_frames[last_index];
}

uint32_t dragonfly_actors_get_index(DragonflyActors* actors, uint32_t plane_id) {
	for(uint32_t i = 0; i < actors->count; i++) {
		if(actors->plane_ids[i] == plane_id) {
			return i;
		}
	}
	return UINT32_MAX;
}
