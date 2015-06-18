#ifndef GAME_H
#define GAME_H

#include "std.h"
#include "math.h"
#include "gl_util.h"

struct InputData;
struct RenderContext;

struct GameMemory
{
	void *memory;
	uint64 size;
};

struct GameState
{
	Texture texture;
	float anim_x;
	float time;
	bool32 initialized;
};

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, 
	RenderContext *render_context, float delta);

#endif // GAME_H