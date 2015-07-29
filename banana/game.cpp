#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>
#include "game.h"
#include "input.h"
#include "rendering.h"
#include "voxels.h"

global_variable float rot = 0.0f;
global_variable Vector3 y_axis = Vector3(0.0f, 1.0f, 0.0f);

void MoveCamera(Camera *camera, Vector3 dir, float amount)
{
	camera->position += dir * amount;
}

Vector3 CameraLeft(Camera *camera)
{
	Vector3 left = Cross(camera->forward, camera->up);
	Normalize(left);
	return left;
}

Vector3 CameraRight(Camera *camera)
{
	Vector3 right = Cross(camera->up, camera->forward);
	Normalize(right);
	return right;
}

void CameraRotateY(Camera *camera, float angle)
{
	Vector3 h_axis = Cross(y_axis, camera->forward);
	Normalize(h_axis);

	Rotate(camera->forward, angle, y_axis);
	Normalize(camera->forward);

	camera->up = Cross(camera->forward, h_axis);
	Normalize(camera->up);
}

void CameraRotateX(Camera *camera, float angle)
{
	Vector3 h_axis = Cross(y_axis, camera->forward);
	Normalize(h_axis);

	Rotate(camera->forward, angle, CameraRight(camera));
	Normalize(camera->forward);

	camera->up = Cross(camera->forward, h_axis);
	Normalize(camera->up);
}

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context,
	VoxelRenderContext *voxel_render_context, WindowData *window_data, bool &paused, float delta)
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

		game->camera = { Vector3(0.0f, 5.0f, -20.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f) };

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
		
		Vector2 center = Vector2((float)window_data->target_width*0.5f, (float)window_data->target_height*0.5f);

		Vector2 mouse_delta = input->mouse_pos - center;

		/*CameraRotateX(&game->camera, (mouse_delta.y) * 5 * delta);
		CameraRotateY(&game->camera, (mouse_delta.x) * 5 * delta);*/

		if (IsKeyDown(input, "right"))
			CameraRotateY(&game->camera, 100 * delta);
		if (IsKeyDown(input, "left"))
			CameraRotateY(&game->camera, -100 * delta);
		if (IsKeyDown(input, "up"))
			CameraRotateX(&game->camera, -100 * delta);
		if (IsKeyDown(input, "down"))
			CameraRotateX(&game->camera, 100 * delta);

		if (IsKeyDown(input, "w"))
			MoveCamera(&game->camera, game->camera.forward, 10 * delta);
		if (IsKeyDown(input, "s"))
			MoveCamera(&game->camera, game->camera.forward, -10 * delta);
		if (IsKeyDown(input, "a"))
			MoveCamera(&game->camera, CameraLeft(&game->camera), 10 * delta);
		if (IsKeyDown(input, "d"))
			MoveCamera(&game->camera, CameraRight(&game->camera), 10 * delta);

		/*if (IsKeyDown(input, "right"))
			rot -= 100.0f * delta;
		if (IsKeyDown(input, "left"))
			rot += 100.0f * delta;*/

		SetMousePosition(input, Vector2(0.5f, 0.5f));
	}
	// Rendering
	RenderClear(render_context, 50, 203, 255, 255, GL_DEPTH_BUFFER_BIT);
	// Matrix4_lookat(game->camera.position, game->camera.position + game->camera.forward, game->camera.up)
	Matrix4 camera_mat = Matrix4_translate(-game->camera.position.x, -game->camera.position.y,
		-game->camera.position.z) * Matrix4_lookat(game->camera.position, game->camera.forward,
		game->camera.up);

	BeginModelRenderer(voxel_render_context, camera_mat);
	glUniform3f(glGetUniformLocation(voxel_render_context->diffuse.program, "light_direction"),
		-1.0f, -1.0f, 1.0f);
	RenderModel(voxel_render_context, &game->model, Vector3(10.0f, game->model.size.y*0.5f, 0.0f), Vector3(1.0f, 1.0f, 1.0f),
		Vector3(0.0f, rot, 0.0f));
	EndModelRenderer();

	BeginVoxelRenderer(voxel_render_context, camera_mat);
	RenderVoxel(voxel_render_context, Vector3(0.0f, -0.5f, 0.0f), Vector3(100.0f, 1.0f, 50.0f), 0, 150, 0, 255);
	RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, rot, 0.0f), 255, 0, 0, 255);
	//RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), 255, 0, 0, 255);
	EndVoxelRenderer();

	BeginRenderer(render_context);
	std::string p = "X: " + std::to_string(game->camera.position.x) + " Y: " + 
		std::to_string(game->camera.position.y) + " Z: " + std::to_string(game->camera.position.z);
	RenderString(render_context, 40.0f, 60.0f, p.c_str(), 0.0f);

	std::string f = "X: " + std::to_string(game->camera.forward.x) + " Y: " +
		std::to_string(game->camera.forward.y) + " Z: " + std::to_string(game->camera.forward.z);
	RenderString(render_context, 40.0f, 80.0f, f.c_str(), 0.0f);

	std::string u = "X: " + std::to_string(game->camera.up.x) + " Y: " +
		std::to_string(game->camera.up.y) + " Z: " + std::to_string(game->camera.up.z);
	RenderString(render_context, 40.0f, 100.0f, u.c_str(), 0.0f);

	Vector3 cross = Cross(y_axis, game->camera.forward);
	std::string c = "X: " + std::to_string(cross.x) + " Y: " +
		std::to_string(cross.y) + " Z: " + std::to_string(cross.z);
	RenderString(render_context, 40.0f, 120.0f, c.c_str(), 0.0f);

	EndRenderer();
}