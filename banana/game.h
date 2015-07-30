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

struct WindowData
{
	int width;
	int height;
	int target_width;
	int target_height;
	int vp_x;
	int vp_y;
	int vp_width;
	int vp_height;
	bool fullscreen;
	bool vsync = false;
};

struct GameMemory
{
	void *memory;
	uint64 size;
};

#define MAP_W 60
#define MAP_H 34

struct Camera
{
	Vector3 position;
	Quaternion rotation;
};

struct GameState
{
	float time;
	World world;
	bool render_aabbs = false;
	uint32 player;
	GLint color_change_loc;
	Texture paul;
	Model model;
	Model maze;
	Model sword;
	Model gimble;
	Quaternion quat;

	Camera camera;

	bool32 initialized;
};

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context,
	VoxelRenderContext *voxel_render_context, WindowData *window, bool &paused, float delta);

#endif // GAME_H