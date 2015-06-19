#include <ctime>
#include "game.h"
#include "input.h"
#include "rendering.h"

internal int rand_range(int min, int max)
{
	return min + (rand() % (int)(max - min + 1));
}

internal int GetSurroundingWallCount(GameState *game, int grid_x, int grid_y)
{
	int wall_count = 0;
	for (int neighbour_x = grid_x - 1; neighbour_x <= grid_x + 1; ++neighbour_x)
	{
		for (int neighbour_y = grid_y - 1; neighbour_y <= grid_y + 1; ++neighbour_y)
		{
			if (neighbour_x >= 0 && neighbour_x < MAP_W && neighbour_y >= 0 && neighbour_y < MAP_H)
			{
				if (neighbour_x != grid_x, neighbour_y != grid_y)
					wall_count += game->map[neighbour_y][neighbour_x];
			}
			else
			{
				++wall_count;
			}
		}
	}
	return wall_count;
}

internal void GenerateMap(GameState *game)
{
	srand(time(0));
	for (int x = 0; x < MAP_W; ++x)
	{
		for (int y = 0; y < MAP_H; ++y)
		{
			if (x == 0 || x == MAP_W - 1 || y == 0 || y == MAP_H - 1)
				game->map[y][x] = 1;
			else
				game->map[y][x] = (rand_range(0, 100) < 76) ? 1 : 0;
		}
	}

	// Smoothing
	for (int i = 0; i < 3; ++i)
	{
		for (int x = 0; x < MAP_W; ++x)
		{
			for (int y = 0; y < MAP_H; ++y)
			{
				int neighbour_wall_tiles = GetSurroundingWallCount(game, x, y);
				if (neighbour_wall_tiles > 4)
					game->map[y][x] = 1;
				else if (neighbour_wall_tiles < 4)
					game->map[y][x] = 0;
			}
		}
	}
}

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context, float delta)
{
	GameState *game = (GameState *)game_memory->memory;
	if (!game->initialized)
	{
		// Initailzation
		game->texture = LoadTexture("assets/sheet.png");
		GenerateMap(game);
		game->camera_pos = Vector2(0.0f, 0.0f);
		game->camera_scale = 3.6f;
		game->initialized = true;
	}

	// Update
	game->time += delta;
	if (IsKeyPressed(input, SDL_SCANCODE_SPACE))
	{
		GenerateMap(game);
	}

	if (IsKeyDown(input, SDL_SCANCODE_RIGHT))
		game->camera_pos.x += 300 * delta;
	if (IsKeyDown(input, SDL_SCANCODE_LEFT))
		game->camera_pos.x -= 300 * delta;
	if (IsKeyDown(input, SDL_SCANCODE_DOWN))
		game->camera_pos.y += 300 * delta;
	if (IsKeyDown(input, SDL_SCANCODE_UP))
		game->camera_pos.y -= 300 * delta;

	if (IsKeyDown(input, SDL_SCANCODE_W))
		game->camera_scale += 1 * delta;
	if (IsKeyDown(input, SDL_SCANCODE_S))
		game->camera_scale -= 1 * delta;
	
	// Rendering
	RenderClear(render_context, 32, 20, 41, 255);
	BeginRenderer(render_context, Matrix4_scale(game->camera_scale, game->camera_scale, 1.0f) *
		Matrix4_translate(-game->camera_pos.x, -game->camera_pos.y, 0.0f));

	for (int x = 0; x < MAP_W; ++x)
	{
		for (int y = 0; y < MAP_H; ++y)
		{
			if (game->map[y][x] == 1)
			{
				Vector2 dim(1920.0f + 16.0f*game->camera_scale, 1080.0f + 16.0f*game->camera_scale);
				Vector2 pos(16.0f + (x * 32.0f), 16.0f + (y * 32.0f));
				if (pos * game->camera_scale >= game->camera_pos - Vector2(16.0f*game->camera_scale, 16.0f*game->camera_scale) &&
					pos * game->camera_scale <= game->camera_pos + dim)
					RenderTexture(render_context, pos.x, pos.y, 0.0f, &game->texture, 64.0f, 224.0f, 32.0f, 32.0f);
			}
		}
	}

	EndRenderer();
	
}