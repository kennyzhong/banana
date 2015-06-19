#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "std.h"
#include "math.h"

enum AABBS
{
	AABB_NONE = 0,
	AABB_WALL = 1 << 0
};

struct AABB
{
	Vector2 min;
	Vector2 max;
	uint32 mask;
};

#endif // COMPONENTS_H