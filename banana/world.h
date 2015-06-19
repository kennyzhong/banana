#ifndef WORLD_H
#define WORLD_H

#include "std.h"
#include "components.h"

#define ENTITY_COUNT 300
#define AABB_COUNT 1000

struct InputData;
struct RenderContext;

struct World
{
	uint32 mask[ENTITY_COUNT];
	Transform transforms[ENTITY_COUNT];
	Velocity velocities[ENTITY_COUNT];
	Sprite sprites[ENTITY_COUNT];
	ComAABB com_aabbs[ENTITY_COUNT];

	AABB aabbs[AABB_COUNT];
};

AABB * CreateAABB(World *world, Vector2 min, Vector2 max, uint32 mask);
bool Intersects(float a_minx, float a_miny, float a_maxx, float a_maxy, 
	float b_minx, float b_miny, float b_maxx, float b_maxy);
bool Intersects(Vector2 a_min, Vector2 a_max, Vector2 b_min, Vector2 b_max);
bool Intersects(AABB a, AABB b);

uint32 CreateEntity(World *world);
void DestroyEntity(World *world, uint32 entity);
bool HasComponent(World *world, uint32 index, uint32 mask);

#define PLAYER_MASK (COMPONENT_TRANSFORM | COMPONENT_SPRITE | \
					COMPONENT_VELOCITY | COMPONENT_AABB) 
uint32 CreatePlayer(World *world, Vector2 position, Texture *entities);
void PlayerUpdate(World *world, uint32 entity, float delta, InputData *input);
void VelocityUpdate(World *world, uint32 entity, float delta);
void AABBUpdate(World *world, uint32 entity);

void SpriteRender(World *world, uint32 entity, RenderContext *render_context);

#endif // WORLD_H