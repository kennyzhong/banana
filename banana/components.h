#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "std.h"
#include "math.h"
#include "gl_util.h"

enum AABBS
{
	AABB_NONE = 0,
	AABB_WALL = 1 << 0,
	AABB_PLAYER = 1 << 1,
};

struct AABB
{
	Vector2 min;
	Vector2 max;
	uint32 mask;
};

enum Components
{
	COMPONENT_NONE = 0,
	COMPONENT_TRANSFORM = 1 << 0,
	COMPONENT_VELOCITY = 1 << 1,
	COMPONENT_SPRITE = 1 << 2,
	COMPONENT_PLAYER = 1 << 3,
	COMPONENT_AABB = 1 << 4
};

struct Transform
{
	Vector2 position;
	float rotation;
	Vector2 scale;
};

struct ComAABB
{
	AABB *aabb;
};

struct Velocity
{
	Vector2 velocity;
	float speed;
	float friction;
	bool grounded;
};

struct Sprite
{
	Texture *texture;
	float x_offset;
	float y_offset;
	float width;
	float height;
};

#endif // COMPONENTS_H