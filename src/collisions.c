#include<stdint.h>
#include<math.h>
#include"collisions.h"

bool collide_rect_and_rect(Vec2 pos1, Vec2 size1, float orient_rad1, Vec2 pos2, Vec2 size2, float orient_rad2) {
	Vec2 verticies1[4] = {
		pos1.x - size1.x/2, pos1.y - size1.y/2,
		pos1.x - size1.x/2, pos1.y + size1.y/2,
		pos1.x + size1.x/2, pos1.y + size1.y/2,
		pos1.x + size1.x/2, pos1.y - size1.y/2
	};
	Vec2 verticies2[4] = {
		pos2.x - size2.x/2, pos2.y - size2.y/2,
		pos2.x - size2.x/2, pos2.y + size2.y/2,
		pos2.x + size2.x/2, pos2.y + size2.y/2,
		pos2.x + size2.x/2, pos2.y - size2.y/2
	};

	// Rotate verticies
	for(uint32_t i = 0; i < 4; ++i) {
		float x1 = verticies1[i].x - pos1.x;
		float y1 = verticies1[i].y - pos1.y;

		float x2 = x1*cos(orient_rad1) - y1*sin(orient_rad1);
		float y2 = x1*sin(orient_rad1) + y1*cos(orient_rad1);

		verticies1[i] = (Vec2){pos1.x + x2, pos1.y + y2};
	}

	for(uint32_t i = 0; i < 4; ++i) {
		float x1 = verticies2[i].x - pos2.x;
		float y1 = verticies2[i].y - pos2.x;

		float x2 = x1*cos(orient_rad2) - y1*sin(orient_rad2);
		float y2 = x1*sin(orient_rad2) + y1*cos(orient_rad2);

		verticies2[i] = (Vec2){pos2.x + x2, pos2.y + y2};
	}
	
	// Create the axis
	Vec2 axes1[4] = {
		vec2_normalize((Vec2){verticies1[1].x - verticies1[0].x, verticies1[1].y - verticies1[0].y}),
		vec2_normalize((Vec2){verticies1[2].x - verticies1[1].x, verticies1[2].y - verticies1[1].y}),
		vec2_normalize((Vec2){verticies2[1].x - verticies2[0].x, verticies2[1].y - verticies2[0].y}),
		vec2_normalize((Vec2){verticies2[2].x - verticies2[1].x, verticies2[2].y - verticies2[1].y})
	};

	for(uint32_t i = 0; i < 4; ++i) {

	}
	return true;
}

bool collide_rect_and_circle(Vec2 rect_pos, Vec2 rect_size, float rect_orient, Vec2 circle_pos, float circle_rad) {
	return false;
}
