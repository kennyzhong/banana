#include "world.h"

AABB * CreateAABB(World *world, Vector2 min, Vector2 max, uint32 mask)
{
	AABB aabb = { min, max, mask };
	bool space_left = false;
	int i;
	for (i = 0; i < AABB_COUNT; ++i)
	{
		if (world->aabbs[i].mask == AABB_NONE)
		{
			world->aabbs[i] = aabb;
			space_left = true;
			break;
		}
	}
	if (space_left)
		return &(world->aabbs[i]);
	else
	{
		Error("No more AABB space left");
		return 0;
	}
}