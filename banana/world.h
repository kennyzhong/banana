#ifndef WORLD_H
#define WORLD_H

#include "std.h"
#include "components.h"

#define ENTITY_COUNT 300
#define AABB_COUNT 1000

struct World
{
	uint32 mask[ENTITY_COUNT];

	AABB aabbs[AABB_COUNT];
};

AABB * CreateAABB(World *world, Vector2 min, Vector2 max, uint32 mask);

#endif // WORLD_H