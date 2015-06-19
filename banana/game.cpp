#include <ctime>
#include <fstream>
#include <sstream>
#include "game.h"
#include "input.h"
#include "rendering.h"
#include "tinyxml2.h"

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
			if (neighbour_x > 0 && neighbour_x < MAP_W-1 && neighbour_y > 0 && neighbour_y < MAP_H-1)
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
				game->map[y][x] = (rand_range(0, 100) < 69) ? 1 : 0;
		}
	}

	// Smoothing
	for (int i = 0; i < 2; ++i)
	{
		for (int x = 0; x < MAP_W; ++x)
		{
			for (int y = 0; y < MAP_H; ++y)
			{
				int neighbour_wall_tiles = GetSurroundingWallCount(game, x, y);
				if (neighbour_wall_tiles >= 4)
					game->map[y][x] = 1;
				else if (neighbour_wall_tiles < 4)
					game->map[y][x] = 0;
			}
		}
	}
}

internal Vector2 GetTileFromID(int id, Texture *texture, float tile_size)
{
	Vector2 pos;
	pos.x = tile_size * (id % (int)(texture->width / tile_size));
	pos.y = tile_size * (id / (int)(texture->width / tile_size));
	return pos;
}

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context, float delta)
{
	GameState *game = (GameState *)game_memory->memory;
	if (!game->initialized)
	{
		// Initailzation
		game->texture = LoadTexture("assets/sheet.png");
		game->entities = LoadTexture("assets/entities.png");
		//GenerateMap(game);

		using namespace tinyxml2;
		XMLDocument doc;
		doc.LoadFile("assets/maps/map01.tmx");
		XMLElement *map = doc.FirstChildElement("map");
		XMLElement *tile_layer = map->FirstChildElement("layer");
		XMLElement *data = tile_layer->FirstChildElement("data");
		XMLElement *collisions = tile_layer->NextSiblingElement("objectgroup");
		
		XMLElement *obj1 = collisions->FirstChildElement();
		while (obj1 != 0)
		{
			float x = (float)atoi(obj1->Attribute("x"));
			float y = (float)atoi(obj1->Attribute("y"));
			float x2 = x + (float)atoi(obj1->Attribute("width"));
			float y2 = y + (float)atoi(obj1->Attribute("height"));
			CreateAABB(&game->world, Vector2(x, y), Vector2(x2, y2), AABB_WALL);
			obj1 = obj1->NextSiblingElement();
		}
		std::stringstream file;
		file << data->GetText();
		for (int row = -1; row < MAP_H; ++row)
		{
			std::string line;
			std::getline(file, line);
			if (!file.good())
				break;
			std::stringstream iss(line);
			for (int col = 0; col < MAP_W; ++col)
			{
				std::string val;
				std::getline(iss, val, ',');
				if (!iss.good())
					break;
				std::stringstream convertor(val);
				// For some reason it offsets by 1??
				int temp_var;
				convertor >> temp_var;
				game->map[row][col] = temp_var - 1;
				// add aabb
			}
		}

		for (int i = 0; i < AABB_COUNT; ++i)
			if (game->world.aabbs[i].mask != AABB_NONE)
		SDL_Log("%0.2f, %0.2f, %0.2f, %0.2f", game->world.aabbs[i].min.x, game->world.aabbs[i].min.y,
			game->world.aabbs[i].max.x, game->world.aabbs[i].max.y);

		game->camera_pos = Vector2(0.0f, 0.0f);
		game->camera_scale = 2.6f;
		game->initialized = true;
	}

	// Update
	game->time += delta;
	if (IsKeyPressed(input, SDL_SCANCODE_SPACE))
	{
		//GenerateMap(game);
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
			if (game->map[y][x] > 0)
			{
				Vector2 dim(1920.0f + 16.0f*game->camera_scale, 1080.0f + 16.0f*game->camera_scale);
				Vector2 pos(16.0f + (x * 32.0f), 16.0f + (y * 32.0f));
				if (pos * game->camera_scale >= game->camera_pos - Vector2(16.0f*game->camera_scale, 16.0f*game->camera_scale) &&
					pos * game->camera_scale <= game->camera_pos + dim)
				{
					Vector2 tile = GetTileFromID(game->map[y][x], &game->texture, 32);
					RenderTexture(render_context, pos.x, pos.y, 0.0f, &game->texture, tile.x, tile.y, 32.0f, 32.0f);
				}
			}
		}
	}

	for (int i = 0; i < AABB_COUNT; ++i)
	{
		if (game->world.aabbs[i].mask != AABB_NONE)
		{
			float width = game->world.aabbs[i].max.x - game->world.aabbs[i].min.x;
			float height = game->world.aabbs[i].max.y - game->world.aabbs[i].min.y;
			float x = game->world.aabbs[i].min.x + width*0.5f;
			float y = game->world.aabbs[i].min.y + height*0.5f;
			RenderSquare(render_context, x, y, width, height, 0.0f, 255, 0, 0, 150, true);
		}
	}

	EndRenderer();
	
}