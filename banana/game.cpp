#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include "game.h"
#include "input.h"
#include "rendering.h"
#include "voxels.h"
#include "tinyxml2.h"

internal Vector2 GetTileFromID(int id, Texture *texture, float tile_size)
{
	Vector2 pos;
	pos.x = tile_size * (id % (int)(texture->width / tile_size));
	pos.y = tile_size * (id / (int)(texture->width / tile_size));
	return pos;
}

global_variable int instance_num = 0;
global_variable GLuint world_vbo, world_vao, world_ebo, world_tbo, world_tobo;

global_variable float rot = 0.0f;

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context,
	VoxelRenderContext *voxel_render_context, bool &paused, float delta)
{
	GameState *game = (GameState *)game_memory->memory;
	if (!game->initialized)
	{
		// Initailzation
		game->sheet = LoadTexture("assets/sheet.png");
		game->entities = LoadTexture("assets/entities.png");
		game->tile = LoadTexture("assets/player.png");
		game->paul = LoadTexture("assets/paul.png");

		for (int i = 0; i < AABB_COUNT; ++i)
			if (game->world.aabbs[i].mask != AABB_NONE)
		SDL_Log("%0.2f, %0.2f, %0.2f, %0.2f", game->world.aabbs[i].min.x, game->world.aabbs[i].min.y,
			game->world.aabbs[i].max.x, game->world.aabbs[i].max.y);

		game->camera_pos = Vector2(0.0f, 0.0f);
		game->camera_scale = 2.6f;

		game->player = CreatePlayer(&game->world, Vector2(96.0f, 96.0f), &game->entities);
		game->color_change_loc = glGetUniformLocation(render_context->diffuse.program, "color_change");
		game->initialized = true;

		game->red = 1.0f;
		game->green = 1.0f;
		game->blue = 1.0f;

		game->camera3d_pos = Vector3(0.0f, 0.0f, -20.0f);
		game->camera3d_rot = Vector3(0.0f, 0.0f, 0.0f);

		game->world_shader = CreateShader("assets/shaders/world.vert", "assets/shaders/world.frag");

		// FBO stuff
		/*glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glGenTextures(1, &fto);
		glBindTexture(GL_TEXTURE_2D, fto);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fto, 0);
		GLenum draw_buffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, draw_buffers);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Error("Framebuffer error", true);

		fbt.width = 1920;
		fbt.height = 1080;
		fbt.id = fto;*/
		game->mv_model = LoadModel("assets/chr_knight.vox");
		InitializeModel(voxel_render_context, &game->model, &game->mv_model);
	}

	// Update
	if (!paused)
	{
		game->time += delta;
		/*for (uint32 entity = 0; entity < ENTITY_COUNT; ++entity)
		{
			if (HasComponent(&game->world, entity, PLAYER_MASK))
				PlayerUpdate(&game->world, entity, delta, input);
			if (HasComponent(&game->world, entity, COMPONENT_ARROW))
				ArrowUpdate(&game->world, entity, delta, input);
			if (HasComponent(&game->world, entity, COMPONENT_VELOCITY | COMPONENT_TRANSFORM | COMPONENT_AABB))
				VelocityUpdate(&game->world, entity, delta);
			if (HasComponent(&game->world, entity, COMPONENT_TRANSFORM | COMPONENT_AABB))
				AABBUpdate(&game->world, entity);
			if (HasComponent(&game->world, entity, COMPONENT_TRANSFORM))
				TransformUpdate(&game->world, entity);
		}
		
		Vector2 position = game->world.transforms[game->player].position;
		Vector2 target;*/
		//game->camera_pos.x = position.x - (1920.0f / game->camera_scale)*0.5f;
		//game->camera_pos.y = position.y - (1080.0f / game->camera_scale)*0.5f;
		/*game->camera_pos.x = position.x - 300.0f;
		game->camera_pos.y = position.y - 300.0f;*/
		//target.x;
		//target.y;
		//game->camera_pos.x += ((target.x - game->camera_pos.x) * 100 * delta);
		//game->camera_pos.y += ((target.y - game->camera_pos.y) * 100 * delta);

		if (IsKeyPressed(input, "f9"))
			game->render_aabbs = !game->render_aabbs;

		if (IsKeyPressed(input, "f1"))
		{
			if (game->red == 1) game->red = 0;
			else game->red = 1;
		}

		if (IsKeyPressed(input, "f2"))
		{
			if (game->green == 1) game->green = 0;
			else game->green = 1;
		}


		if (IsKeyPressed(input, "f3"))
		{
			if (game->blue == 1) game->blue = 0;
			else game->blue = 1;
		}

		if (IsKeyDown(input, "w"))
			game->camera3d_pos.z += 10.0f * delta;
		if (IsKeyDown(input, "s"))
			game->camera3d_pos.z -= 10.0f * delta;
		if (IsKeyDown(input, "a"))
			game->camera3d_pos.x += 10.0f * delta;
		if (IsKeyDown(input, "d"))
			game->camera3d_pos.x -= 10.0f * delta;
		if (IsKeyDown(input, "space"))
			game->camera3d_pos.y -= 10.0f * delta;
		if (IsKeyDown(input, "left shift"))
			game->camera3d_pos.y += 10.0f * delta;

		if (IsKeyDown(input, "right"))
			game->camera3d_rot.y += 20 * delta;
		if (IsKeyDown(input, "left"))
			game->camera3d_rot.y -= 20 * delta;
		if (IsKeyDown(input, "up"))
			game->camera3d_rot.x += 20 * delta;
		if (IsKeyDown(input, "down"))
			game->camera3d_rot.x -= 20 * delta;

		/*if (game->camera_pos.x < 0) game->camera_pos.x = 0;
		if (game->camera_pos.y < 0) game->camera_pos.y = 0;
		if (game->camera_pos.x + 1920.0f / game->camera_scale > MAP_W*32.0f + 16.0f)
			game->camera_pos.x = (MAP_W*32.0f + 16.0f) - 1920.0f / game->camera_scale;
		if (game->camera_pos.y + 1080.0f / game->camera_scale > MAP_H*32.0f + 16.0f)
			game->camera_pos.y = (MAP_H*32.0f + 16.0f) - 1080.0f / game->camera_scale;*/

		rot += 100.0f * delta;
	}
	// Rendering
	RenderClear(render_context, 32, 20, 41, 255, GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	/*glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);*/

	BeginVoxelRenderer(voxel_render_context, 
		Matrix4_translate(game->camera3d_pos.x, game->camera3d_pos.y, game->camera3d_pos.z));
	/*RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), 
		game->camera3d_rot, 255, 0, 0, 255, false);*/
	/*for (int i = 0; i < game->mv_model.num_voxels; ++i)
	{
		MV_Voxel vox = game->mv_model.voxels[i];
		RenderVoxel(voxel_render_context, Vector3((float)vox.x, (float)vox.y, (float)vox.z),
			Vector3(1.0f, 1.0f, 1.0f),  255, 0, 0, 255);
	}*/
	RenderModel(voxel_render_context, &game->model, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f));
	EndVoxelRenderer();

	//RenderSquare(render_context, game->camera_pos.x, game->camera_pos.y, 32.0f, 32.0f, 255, 255, 170, 255);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//BeginRenderer(render_context);
	//RenderTexture(render_context, 700.0f, 700.0f, &game->paul);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//RenderTexture(render_context, 1920.0f*0.5f, 1080.0f*0.5f, &fbt);
	//EndRenderer();	
}