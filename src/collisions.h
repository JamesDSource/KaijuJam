#ifndef COLLISIONS__
#define COLLISIONS__

#include<stdbool.h>
#include"vec2.h"

bool collide_rect_and_rect(Vec2 pos1, Vec2 size1, float orient1, Vec2 pos2, Vec2 size2, float orient2);
bool collide_rect_and_circle(Vec2 rect_pos, Vec2 rect_size, float rect_orient, Vec2 circle_pos, float circle_rad);

#endif
