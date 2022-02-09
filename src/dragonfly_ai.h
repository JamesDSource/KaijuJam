#ifndef DRAGONFLY_AI
#define DRAGONFLY_AI

#include<stdint.h>

#define DF_FIREBALL_COOLDOWN 40

typedef struct {
	uint32_t count, allocated;
	uint32_t* plane_ids;
	uint32_t* fireball_cooldowns;
	float* current_frames;
}DragonflyActors;

DragonflyActors* dragonfly_actors_create();
void dragonfly_actors_destroy(DragonflyActors* actors);
void dragonfly_actors_add(DragonflyActors* actors, uint32_t plane_id);
void dragonfly_actors_remove(DragonflyActors* actors, uint32_t index);
uint32_t dragonfly_actors_get_index(DragonflyActors* actors, uint32_t plane_id);
void dragonfly_actors_update(DragonflyActors* actors);

#endif
