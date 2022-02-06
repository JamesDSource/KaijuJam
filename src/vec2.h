#ifndef VEC2__
#define VEC2__

typedef struct {
	float x, y;
}Vec2;

float vec2_dot(Vec2 v1, Vec2 v2);
float vec2_length(Vec2 v);
Vec2 vec2_normalize(Vec2 v);

#endif
