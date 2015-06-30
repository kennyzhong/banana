#include "world.h"
#include "input.h"
#include "rendering.h"

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

bool Intersects(float a_minx, float a_miny, float a_maxx, float a_maxy,
	float b_minx, float b_miny, float b_maxx, float b_maxy)
{
	return Intersects(Vector2(a_minx, a_miny), Vector2(a_maxx, a_maxy),
		Vector2(b_minx, b_miny), Vector2(b_maxx, b_maxy));
}

bool Intersects(Vector2 a_min, Vector2 a_max, Vector2 b_min, Vector2 b_max)
{
	if (a_min.x < b_max.x && a_max.x > b_min.x && a_min.y < b_max.y && a_max.y > b_min.y)
		return true;
	return false;
}

bool Intersects(AABB a, AABB b)
{
	return Intersects(a.min, a.max, b.min, b.max);
}

uint32 CreateEntity(World *world)
{
	uint32 entity;
	for (entity = 0; entity < ENTITY_COUNT; ++entity)
	{
		if (world->mask[entity] == COMPONENT_NONE)
			return entity;
	}
	Error("No more entity space left");
	return ENTITY_COUNT;
}

void DestroyEntity(World *world, uint32 entity)
{
	world->mask[entity] = COMPONENT_NONE;
}

bool HasComponent(World *world, uint32 index, uint32 mask)
{
	return ((world->mask[index] & mask) == mask);
}

uint32 CreatePlayer(World *world, Vector2 position, Texture *entities)
{
	uint32 entity = CreateEntity(world);
	world->mask[entity] = PLAYER_MASK;
	Vector2 dim(11.0f, 30.0f);
	world->transforms[entity] = { position };
	world->sprites[entity] = { entities, 0.0f, 0.0f, 32.0f, 32.0f };
	world->com_aabbs[entity].aabb = CreateAABB(world, position - (dim*0.5f), position + (dim*0.5f), AABB_PLAYER);
	world->velocities[entity] = { Vector2(), 200.0f, 0.7f };
	return entity;
}

uint32 CreateArrow(World *world, Vector2 position, Texture *entities, uint32 entity_owner)
{
	uint32 entity = CreateEntity(world);
	world->mask[entity] = ARROW_MASK;
	world->owners[entity] = entity_owner;
	Vector2 dim(13.0f, 5.0f);
	world->transforms[entity] = { position };
	world->sprites[entity] = { entities, 0.0f, 64.0f, 16.0f, 16.0f };
	world->com_aabbs[entity].aabb = CreateAABB(world, position - (dim*0.5f), position + (dim*0.5f), AABB_PLAYER);
	world->velocities[entity] = { Vector2(), 200.0f, 0.7f };
	world->arrows[entity] = { false };
	return entity;
}

void PlayerUpdate(World *world, uint32 entity, float delta, InputData *input)
{
	Transform *t = &world->transforms[entity];
	Velocity *v = &world->velocities[entity];
	Sprite *s = &world->sprites[entity];

	if (IsKeyDown(input, "d"))
	{
		v->velocity.x += v->speed;
		s->y_offset = 0.0f;
	}
	if (IsKeyDown(input, "a"))
	{
		v->velocity.x -= v->speed;
		s->y_offset = 32.0f;
	}

	if (IsKeyDown(input, "w"))
	{
		v->velocity.y -= v->speed;
	}
	if (IsKeyDown(input, "s"))
	{
		v->velocity.y += v->speed;
	}

	if (IsMousePressed(input, MB_LEFT))
	{
		CreateArrow(world, t->position, s->texture, entity);
	}
}

void ArrowUpdate(World *world, uint32 entity, float delta, InputData *input)
{
	Transform *t = &world->transforms[entity];
	Velocity *v = &world->velocities[entity];
	Arrow *a = &world->arrows[entity];

	if (!a->fired)
		t->rotation = atan2f(input->mouse_pos.y - t->position.y, input->mouse_pos.x - t->position.y) * 180.0f / M_PI;

	if (IsMouseReleased(input, MB_LEFT) && !a->fired)
	{
		a->fired = true;
	}
	if (IsMouseUp(input, MB_LEFT) && !a->fired)
		DestroyEntity(world, entity);

	if (a->fired)
	{
		v->velocity.x = (float)cosf(ToRadians(t->rotation)) * v->speed;
		v->velocity.y = (float)sinf(ToRadians(t->rotation)) * v->speed;
	}
}

void TransformUpdate(World *world, uint32 entity)
{
	Transform *t = &world->transforms[entity];
	uint32 owner_id = world->owners[entity];
	if (owner_id != 0)
	{
		Transform *t_owner = &world->transforms[owner_id];
		t->position = t_owner->position + t->position;
	}
}

void VelocityUpdate(World *world, uint32 entity, float delta)
{
	Transform *t = &world->transforms[entity];
	Velocity *v = &world->velocities[entity];
	ComAABB *aabb = &world->com_aabbs[entity];
	Vector2 dim = aabb->aabb->max - aabb->aabb->min;
	float height = dim.y;
	float width = dim.x;

	for (Uint32 i = 0; i < AABB_COUNT; ++i)
	{
		AABB other = world->aabbs[i];
		if (other.mask == AABB_WALL)
		{
			// Right collision
			if (Intersects(
				t->position.x + width*0.5f,
				t->position.y - height*0.5f,
				t->position.x + width*0.5f + v->velocity.x * delta,
				t->position.y + height*0.5f,
				other.min.x, other.min.y, other.max.x, other.max.y))
			{
				while (!Intersects(
					t->position.x + width*0.5f,
					t->position.y - height*0.5f,
					t->position.x + width*0.5f + 1,
					t->position.y + height*0.5f,
					other.min.x, other.min.y, other.max.x, other.max.y))
					t->position.x += 1;
				v->velocity.x = 0;
			}

			// Left Collision
			if (Intersects(
				t->position.x - width*0.5f + v->velocity.x * delta,
				t->position.y - height*0.5f,
				t->position.x - width*0.5f,
				t->position.y + height*0.5f,
				other.min.x, other.min.y, other.max.x, other.max.y))
			{
				while (!Intersects(
					t->position.x - width*0.5f - 1,
					t->position.y - height*0.5f,
					t->position.x - width*0.5f,
					t->position.y + height*0.5f,
					other.min.x, other.min.y, other.max.x, other.max.y))
					t->position.x -= 1;
				v->velocity.x = 0;
			}

			// Bottom Collision
			if (Intersects(
				t->position.x - width*0.5f,
				t->position.y + height*0.5f,
				t->position.x + width*0.5f,
				t->position.y + height*0.5f + v->velocity.y * delta,
				other.min.x, other.min.y, other.max.x, other.max.y))
			{
				while (!Intersects(
					t->position.x - width*0.5f,
					t->position.y + height*0.5f,
					t->position.x + width*0.5f,
					t->position.y + height*0.5f + 1,
					other.min.x, other.min.y, other.max.x, other.max.y))
					t->position.y += 1;
				v->velocity.y = 0;
				v->grounded = true;
			}

			// Top Collision
			if (Intersects(
				t->position.x - width*0.5f,
				t->position.y - height*0.5f + v->velocity.y * delta,
				t->position.x + width*0.5f,
				t->position.y - height*0.5f,
				other.min.x, other.min.y, other.max.x, other.max.y))
			{
				while (!Intersects(
					t->position.x - width*0.5f,
					t->position.y - height*0.5f - 1,
					t->position.x + width*0.5f,
					t->position.y - height*0.5f,
					other.min.x, other.min.y, other.max.x, other.max.y))
					t->position.y -= 1;
				v->velocity.y = 0;
			}
		}
	}

	v->velocity *= v->friction;
	t->position += (v->velocity * delta);
}

void AABBUpdate(World *world, uint32 entity)
{
	Transform *t = &world->transforms[entity];
	AABB *aabb = (&world->com_aabbs[entity])->aabb;

	Vector2 dim = aabb->max - aabb->min;
	aabb->min = t->position - (dim * 0.5f);
	aabb->max = t->position + (dim * 0.5f);
}

 // Rendering component updates
void SpriteRender(World *world, uint32 entity, RenderContext *render_context)
{
	Transform *t = &world->transforms[entity];
	Sprite *s = &world->sprites[entity];

	RenderTexture(render_context, t->position.x, t->position.y, t->rotation, s->texture,
		s->x_offset, s->y_offset, s->width, s->height);
}