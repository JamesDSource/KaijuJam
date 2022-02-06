#include<math.h>
#include"vec2.h"

float vec2_dot(Vec2 v1, Vec2 v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

float vec2_length(Vec2 v) {
	return sqrtf(v.x*v.x + v.y*v.y);
}

Vec2 vec2_normalize(Vec2 v) {
	float il = 1/sqrtf(v.x*v.x + v.y*v.y);	
	return (Vec2){v.x/il, v.y/il};
}
