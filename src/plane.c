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
	planes->velocities = malloc(sizeof(*planes->velocities)*alloc);
	planes->targets = malloc(sizeof(*planes->targets)*alloc);
	planes->flags = malloc(sizeof(*planes->flags)*alloc);
	planes->types = malloc(sizeof(*planes->types)*alloc);
	return planes;
}

void planes_free(Planes* planes) {
	free(planes->ids);
	free(planes->positions);
	free(planes->velocities);
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
		planes->velocities = realloc(planes->velocities, sizeof(*planes->velocities)*alloc);
		planes->targets = realloc(planes->targets, sizeof(*planes->targets)*alloc);
		planes->flags = realloc(planes->flags, sizeof(*planes->flags)*alloc);
		planes->types = realloc(planes->types, sizeof(*planes->types)*alloc);
	}
	
	planes->ids[i] = next_id;
	planes->positions[i] = (Vec2){0, 0};
	planes->velocities[i] = (PlaneVel){
		.accel = 0.1,
		.turn_accel = .5,
		.max_speed = 3,
		.max_turn = 4,
		.dir = 0,
		.speed = 0,
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
			PlaneVel* vel = planes->velocities + i;
			Vec2 target = planes->targets[i];
			Vec2* pos = planes->positions + i;

			float angle_to_target = atan2(target.y - pos->y, target.x - pos->x)*(180/M_PI);
			if(angle_to_target < 0) {
				angle_to_target += 360;
			}
			// Interpolate turn
			float angle_dif = angle_to_target - vel->dir;
			if(vel->turn < angle_dif) {
				vel->turn += vel->turn_accel; 
				if(vel->turn > angle_dif) {
					vel->turn = angle_dif;
				}
			} else { 
				vel->turn -= vel->turn_accel;
				if(vel->turn < angle_dif) {
					vel->turn = angle_dif;
				}
			}
			// Cap out the turn
			if(fabs(vel->turn) > vel->max_turn) {
				vel->turn = vel->max_turn*(vel->turn/fabs(vel->turn));
			}

			if(fabs(vel->speed - vel->max_speed) < vel->accel) {
				vel->speed = vel->max_speed;
			}
			else if(vel->speed > vel->max_speed) {
				vel->speed -= vel->accel;
			}
			else {
				vel->speed += vel->accel;
			}

			vel->dir += vel->turn;
			while(vel->dir > 360) {
				vel->dir -= 360;
			}
			while(vel->dir < 0) {
				vel->dir += 360;
			}
			float a = vel->dir*(M_PI/180);

			vel->momentum = (Vec2){cos(a)*vel->speed, sin(a)*vel->speed};
			pos->x += vel->momentum.x;
			pos->y += vel->momentum.y;
		}
	}
}
