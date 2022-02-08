#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include"plane.h"
#include"main.h"

const float GRAVITY = 0.1;

Planes* planes_init() {
	Planes* planes = malloc(sizeof(Planes));
	const uint32_t alloc = 3;
	planes->allocated = alloc;
	planes->count = 0;

	planes->ids = malloc(sizeof(*planes->ids)*alloc);
	planes->positions = malloc(sizeof(*planes->positions)*alloc);
	planes->collider_sizes = malloc(sizeof(*planes->collider_sizes)*alloc);
	planes->velocities = malloc(sizeof(*planes->velocities)*alloc);
	planes->dirs = malloc(sizeof(*planes->dirs)*alloc);
	planes->targets = malloc(sizeof(*planes->targets)*alloc);
	planes->flags = malloc(sizeof(*planes->flags)*alloc);
	planes->types = malloc(sizeof(*planes->types)*alloc);
	return planes;
}

void planes_free(Planes* planes) {
	free(planes->ids);
	free(planes->positions);
	free(planes->collider_sizes);
	free(planes->velocities);
	free(planes->dirs);
	free(planes->targets);
	free(planes->flags);
	free(planes->types);
	free(planes);
}

uint32_t plane_add(Planes* planes, uint32_t* index) {
	static uint32_t next_id = 0;
	uint32_t i = planes->count++;
	if(planes->count < planes->allocated) {
		const uint32_t alloc = planes->allocated *= 2;
		planes->ids = realloc(planes->ids, sizeof(*planes->ids)*alloc);
		planes->positions = realloc(planes->positions, sizeof(*planes->positions)*alloc);
		planes->collider_sizes = realloc(planes->collider_sizes, sizeof(*planes->collider_sizes)*alloc);
		planes->velocities = realloc(planes->velocities, sizeof(*planes->velocities)*alloc);
		planes->dirs = realloc(planes->dirs, sizeof(*planes->dirs)*alloc);
		planes->targets = realloc(planes->targets, sizeof(*planes->targets)*alloc);
		planes->flags = realloc(planes->flags, sizeof(*planes->flags)*alloc);
		planes->types = realloc(planes->types, sizeof(*planes->types)*alloc);
	}
	
	planes->ids[i] = next_id;
	planes->positions[i] = (Vec2){0, 0};
	planes->collider_sizes[i] = (Vec2){10, 10};
	planes->velocities[i] = (PlaneVel){
		.turn_accel = .5,
		.max_turn = 4,
		.speed = 5,
		.turn = 0,
		.momentum = (Vec2){0, 0}
	};
	planes->targets[i] = (Vec2){0, 0};
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
			planes->collider_sizes[i] = planes->collider_sizes[last_index];
			planes->velocities[i] = planes->velocities[last_index];
			planes->dirs[i] = planes->dirs[last_index];
			planes->targets[i] = planes->targets[last_index];
			planes->flags[i] = planes->flags[last_index];
			planes->types[i] = planes->types[last_index];
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
			PlaneVel* vel = planes->velocities + i;
			float dir = planes->dirs[i];
			Vec2 target = planes->targets[i];
			Vec2* pos = planes->positions + i;

			float angle_to_target = atan2(target.y - pos->y, target.x - pos->x)*(180/M_PI);
			if(angle_to_target < 0) {
				angle_to_target += 360;
			}
			// Interpolate turn
			float angle_dif = angle_to_target - dir;
			float angle_dif2;
			if(dir > 180) {
				angle_dif2 = 360 - dir + angle_to_target;
			} else {
				angle_dif2 = -dir - (360 - angle_to_target);
			}

			if(fabs(angle_dif2) < fabs(angle_dif)) {
				angle_dif = angle_dif2;
			}

			if(fabs(vel->turn - angle_dif) <= vel->turn_accel) {
				vel->turn = angle_dif;
			} else if(vel->turn < angle_dif) {
				vel->turn += vel->turn_accel; 
			} else { 
				vel->turn -= vel->turn_accel;
			}
			// Cap out the turn
			if(fabs(vel->turn) > vel->max_turn) {
				vel->turn = copysign(vel->max_turn, vel->turn);
			}

			if(fabs(angle_dif) < fabs(vel->turn)) {
				planes->dirs[i] = angle_to_target;
			} else {
				planes->dirs[i] += vel->turn;
			}
			while(dir > 360) {
				 planes->dirs[i] = 360;
			}
			while(dir < 0) {
				planes->dirs[i] += 360;
			}
			float a = dir*(M_PI/180);

			vel->momentum = (Vec2){cos(a)*vel->speed, sin(a)*vel->speed};
			pos->x += vel->momentum.x;
			pos->y += vel->momentum.y;
		}
	}
}
