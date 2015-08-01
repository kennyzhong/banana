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

void CameraRotateY(Camera *camera, float angle)
{
	Quaternion n = Quaternion(angle, y_axis) * camera->rotation;
	Normalize(n);
	camera->rotation = n;
}

void CameraRotateX(Camera *camera, float angle)
{
	Quaternion n = Quaternion(angle, GetRight(camera->rotation)) * camera->rotation;
	Normalize(n);
	camera->rotation = n;
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

		game->camera = { Vector3(0.0f, 5.0f, -20.0f)};

		MV_Model mv_model = LoadModel("assets/chr_knight.vox");
		MV_Model mv_maze = LoadModel("assets/maze2D.vox");
		MV_Model mv_sword = LoadModel("assets/sword.vox");
		MV_Model mv_gimble = LoadModel("assets/gimble.vox");

		InitializeModel(voxel_render_context, &game->maze, &mv_maze);
		InitializeModel(voxel_render_context, &game->model, &mv_model);
		InitializeModel(voxel_render_context, &game->sword, &mv_sword);
		InitializeModel(voxel_render_context, &game->gimble, &mv_gimble);

		//game->quat = Quaternion(0.0f, Vector3(1.0f, 0.0f, 0.0f));

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
		}*/
		
		if (IsKeyPressed(input, "f9"))
			game->render_aabbs = !game->render_aabbs;
		
		Vector2 center = Vector2((float)window_data->target_width*0.5f, (float)window_data->target_height*0.5f);

		Vector2 mouse_delta = input->mouse_pos - center;

		CameraRotateX(&game->camera, (mouse_delta.y) * 5 * delta);
		CameraRotateY(&game->camera, (mouse_delta.x) * 5 * delta);

		if (IsKeyDown(input, "right"))
			CameraRotateY(&game->camera, 100 * delta);
		if (IsKeyDown(input, "left"))
			CameraRotateY(&game->camera, -100 * delta);
		if (IsKeyDown(input, "up"))
			CameraRotateX(&game->camera, -100 * delta);
		if (IsKeyDown(input, "down"))
			CameraRotateX(&game->camera, 100.0f * delta);


		if (IsKeyDown(input, "w"))
			MoveCamera(&game->camera, GetForward(game->camera.rotation), 10 * delta);
		if (IsKeyDown(input, "s"))
			MoveCamera(&game->camera, GetBackward(game->camera.rotation), 10 * delta);
		if (IsKeyDown(input, "a"))
			MoveCamera(&game->camera, GetLeft(game->camera.rotation), 10 * delta);
		if (IsKeyDown(input, "d"))
			MoveCamera(&game->camera, GetRight(game->camera.rotation), 10 * delta);
		if (IsKeyDown(input, "left shift"))
			MoveCamera(&game->camera, y_axis, -10 * delta);
		if (IsKeyDown(input, "space"))
			MoveCamera(&game->camera, y_axis, 10 * delta);

		/*if (IsKeyDown(input, "right"))
			rot -= 100.0f * delta;
		if (IsKeyDown(input, "left"))
			rot += 100.0f * delta;*/

		game->quat = Quaternion(100 * delta, Vector3(0.0f, 1.0f, 0.0f)) * game->quat;

		SetMousePosition(input, Vector2(0.5f, 0.5f));\
	}
	// Rendering
	RenderClear(render_context, 50, 203, 255, 255, GL_DEPTH_BUFFER_BIT);
	// Matrix4_lookat(game->camera.position, game->camera.position + game->camera.forward, game->camera.up)
	Matrix4 camera_mat = Matrix4_translate(-game->camera.position.x, -game->camera.position.y,
		-game->camera.position.z) * Matrix4_rotate(-game->camera.rotation);

	BeginModelRenderer(voxel_render_context, camera_mat);
	SetShaderUniform(&voxel_render_context->diffuse, "light_direction", Vector3(-1.0f, -1.0f, 1.0f));
	RenderModel(voxel_render_context, &game->model, Matrix4_scale(1.0f, 1.0f, 1.0f) * 
		Matrix4_rotate(game->quat) * Matrix4_translate(10.0f, (game->model.size.y * 0.5f), 0.0f));
	EndModelRenderer();

	BeginVoxelRenderer(voxel_render_context, camera_mat);
	RenderVoxel(voxel_render_context, Vector3(0.0f, -0.5f, 0.0f), Vector3(300.0f, 1.0f, 150.0f), 0, 150, 0, 255);
	//RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, rot, 0.0f), 255, 0, 0, 255);
	//RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f), 255, 0, 0, 255);

	RenderVoxel(voxel_render_context, Vector3(), Vector3(1.0f, 1.0f, 1.0f)
		+ (GetForward(game->camera.rotation) * 10), 255, 0, 0, 255);
	EndVoxelRenderer();

	BeginRenderer(render_context);
	std::string p = "X: " + std::to_string(game->camera.position.x) + " Y: " + 
		std::to_string(game->camera.position.y) + " Z: " + std::to_string(game->camera.position.z);
	RenderString(render_context, 40.0f, 60.0f, p.c_str(), 0.0f);

	/*std::string f = "X: " + std::to_string(game->camera.forward.x) + " Y: " +
		std::to_string(game->camera.forward.y) + " Z: " + std::to_string(game->camera.forward.z);
	RenderString(render_context, 40.0f, 80.0f, f.c_str(), 0.0f);

	std::string u = "X: " + std::to_string(game->camera.up.x) + " Y: " +
		std::to_string(game->camera.up.y) + " Z: " + std::to_string(game->camera.up.z);
	RenderString(render_context, 40.0f, 100.0f, u.c_str(), 0.0f);
	*/
	Vector3 cross = GetRight(game->camera.rotation);
	std::string c = "X: " + std::to_string(cross.x) + " Y: " +
		std::to_string(cross.y) + " Z: " + std::to_string(cross.z);
	RenderString(render_context, 40.0f, 120.0f, c.c_str(), 0.0f);

	EndRenderer();
}