#include<float.h>
#include<stdint.h>
#include<math.h>
#include"collisions.h"
#include<stdio.h>

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
	Vec2 axes[4] = {
		vec2_normalize((Vec2){verticies1[1].x - verticies1[0].x, verticies1[1].y - verticies1[0].y}),
		vec2_normalize((Vec2){verticies1[2].x - verticies1[1].x, verticies1[2].y - verticies1[1].y}),
		vec2_normalize((Vec2){verticies2[1].x - verticies2[0].x, verticies2[1].y - verticies2[0].y}),
		vec2_normalize((Vec2){verticies2[2].x - verticies2[1].x, verticies2[2].y - verticies2[1].y})
	};

	for(uint32_t i = 0; i < 4; ++i) {
		Vec2 axis = axes[i];

		float max1 = FLT_MIN;
		float min1 = FLT_MAX;
		for(uint32_t j = 0; j < 4; ++j) {
				float projection = vec2_dot(verticies1[j], axis);		

				if(projection > max1) {
					max1 = projection;
				}
				if(projection < min1) {
					min1 = projection;
				}
		}

		float max2 = FLT_MIN;
		float min2 = FLT_MAX;
		for(uint32_t j = 0; j < 4; ++j) {
				float projection = vec2_dot(verticies2[j], axis);		

				if(projection > max1) {
					max2 = projection;
				}
				if(projection < min1) {
					min2 = projection;
				}
		}

		if(min2 < max1 && min1 < max2) {
			return false;
		}
	}
	return true;
}

bool collide_rect_and_circle(Vec2 rect_pos, Vec2 rect_size, float rect_orient, Vec2 circle_pos, float circle_rad) {
	Vec2 verticies[4] = {
		rect_pos.x - rect_size.x/2, rect_pos.y - rect_size.y/2,
		rect_pos.x - rect_size.x/2, rect_pos.y + rect_size.y/2,
		rect_pos.x + rect_size.x/2, rect_pos.y + rect_size.y/2,
		rect_pos.x + rect_size.x/2, rect_pos.y - rect_size.y/2
	};
	//
	// Rotate verticies
	if(rect_orient != 0) {
		for(uint32_t i = 0; i < 4; ++i) {
			float x1 = verticies[i].x - rect_pos.x;
			float y1 = verticies[i].y - rect_pos.y;

			float x2 = x1*cos(rect_orient) - y1*sin(rect_orient);
			float y2 = x1*sin(rect_orient) + y1*cos(rect_orient);

			verticies[i] = (Vec2){rect_pos.x + x2, rect_pos.y + y2};
		}
	}

	for(uint32_t i = 0; i < 4; ++i) {
		Vec2 vertex = verticies[i];
		Vec2 vertex_next = verticies[(i + 1)%4];
		Vec2 edge_normal = vec2_normalize((Vec2){-(vertex_next.y - vertex.y), vertex_next.x - vertex.x});

		float s = vec2_dot(edge_normal, (Vec2){circle_pos.x - vertex.x, circle_pos.y - vertex.y});
		if(s > circle_rad) {
			return false;
		}
	}
	
	return true;
}
