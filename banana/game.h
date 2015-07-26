#ifndef GAME_H
#define GAME_H

#include "std.h"
#include "math.h"
#include "gl_util.h"
#include "world.h"
#include "mvload.h"
#include "voxels.h"

struct InputData;
struct RenderContext;
struct VoxelRenderContext;

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
	World world;
	bool render_aabbs = false;
	uint32 player;
	GLint color_change_loc;
	Texture paul;
	MV_Model mv_model;
	Model model;

	Vector3 camera3d_pos;
	Vector3 camera3d_rot;

	bool32 initialized;
};

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context,
	VoxelRenderContext *voxel_render_context, bool &paused, float delta);

#endif // GAME_H