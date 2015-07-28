#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include "game.h"
#include "input.h"
#include "rendering.h"
#include "voxels.h"

global_variable float rot = 0.0f;

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context,
	VoxelRenderContext *voxel_render_context, bool &paused, float delta)
{
	GameState *game = (GameState *)game_memory->memory;
	if (!game->initialized)
	{
		// Initailzation
		game->paul = LoadTexture("assets/paul.png");

		for (int i = 0; i < AABB_COUNT; ++i)
			if (game->world.aabbs[i].mask != AABB_NONE)
		SDL_Log("%0.2f, %0.2f, %0.2f, %0.2f", game->world.aabbs[i].min.x, game->world.aabbs[i].min.y,
			game->world.aabbs[i].max.x, game->world.aabbs[i].max.y);

		//game->player = CreatePlayer(&game->world, Vector2(96.0f, 96.0f), &game->entities);

		game->camera3d_pos = Vector3(0.0f, 0.0f, 20.0f);
		game->camera3d_dir = Vector3(0.0f, 0.0f, 1.0f);


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
		game->mv_model = LoadModel("assets/chr_seek.vox");

		game->mv_maze = LoadModel("assets/maze2D.vox");
		InitializeModel(voxel_render_context, &game->maze, &game->mv_maze);
		InitializeModel(voxel_render_context, &game->model, &game->mv_model);

		game->initialized = true;
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

		if (IsKeyDown(input, "w"))
			game->camera3d_pos.z -= 10.0f * delta;
		if (IsKeyDown(input, "s"))
			game->camera3d_pos.z += 10.0f * delta;
		if (IsKeyDown(input, "a"))
			game->camera3d_pos.x -= 10.0f * delta;
		if (IsKeyDown(input, "d"))
			game->camera3d_pos.x += 10.0f * delta;
		if (IsKeyDown(input, "space"))
			game->camera3d_pos.y += 10.0f * delta;
		if (IsKeyDown(input, "left shift"))
			game->camera3d_pos.y -= 10.0f * delta;

		if (IsKeyDown(input, "right"))
			rot -= 100.0f * delta;
		if (IsKeyDown(input, "left"))
			rot += 100.0f * delta;

		//game->camera3d_dir.x = (float)cosf(rot);
		//game->camera3d_dir.z = (float)sinf(rot);
	}
	// Rendering
	RenderClear(render_context, 32, 20, 41, 255, GL_DEPTH_BUFFER_BIT);
	Matrix4 camera_mat = Matrix4_lookat(game->camera3d_pos, game->camera3d_pos + game->camera3d_dir,
		Vector3(0.0f, 1.0f, 0.0f));

	BeginModelRenderer(voxel_render_context, camera_mat);
	RenderModel(voxel_render_context, &game->model, Vector3(0.0f, 2.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, rot, 0.0f));
	RenderModel(voxel_render_context, &game->maze, Vector3(0.0f, -7.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector3());
	EndModelRenderer();
	BeginVoxelRenderer(voxel_render_context, 
		camera_mat);
	/*glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);*/
	//glCullFace(GL_FRONT);
	//RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, rot, 0.0f), 255, 0, 0, 255);
	//glCullFace(GL_BACK);
	/*glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);*/
	//RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), 255, 0, 0, 255);
	EndVoxelRenderer();

	//RenderSquare(render_context, game->camera_pos.x, game->camera_pos.y, 32.0f, 32.0f, 255, 255, 170, 255);
	//glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//BeginRenderer(render_context);
	//RenderTexture(render_context, 700.0f, 700.0f, &game->paul);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//RenderTexture(render_context, 1920.0f*0.5f, 1080.0f*0.5f, &fbt);
	//EndRenderer();	
}