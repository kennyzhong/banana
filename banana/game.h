#ifndef GAME_H
#define GAME_H

#include "std.h"
#include "math.h"
#include "gl_util.h"
#include "world.h"

struct InputData;
struct RenderContext;

struct GameMemory
{
	void *memory;
	uint64 size;
};

#define MAP_W 60
#define MAP_H 34

struct GameState
{
	float time;
	int map[MAP_H][MAP_W];
	Vector2 camera_pos;
	float camera_scale;
	World world;
	Texture sheet;
	Texture entities;
	bool render_aabbs = false;
	uint32 player;
	GLint color_change_loc;
	float red;
	float green;
	float blue;
	bool32 initialized;
};

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, 
	RenderContext *render_context, bool &paused, float delta);

#endif // GAME_H