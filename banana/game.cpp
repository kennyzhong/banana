#include <ctime>
#include <fstream>
#include <sstream>
#include "game.h"
#include "input.h"
#include "rendering.h"
#include "tinyxml2.h"

internal Vector2 GetTileFromID(int id, Texture *texture, float tile_size)
{
	Vector2 pos;
	pos.x = tile_size * (id % (int)(texture->width / tile_size));
	pos.y = tile_size * (id / (int)(texture->width / tile_size));
	return pos;
}

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context,
	bool &paused, float delta)
{
	GameState *game = (GameState *)game_memory->memory;
	if (!game->initialized)
	{
		// Initailzation
		game->sheet = LoadTexture("assets/sheet.png");
		game->entities = LoadTexture("assets/entities.png");

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
		game->camera_scale = 1.0f;

		game->player = CreatePlayer(&game->world, Vector2(100.0f, 100.0f), &game->entities);
		game->color_change_loc = glGetUniformLocation(render_context->diffuse.program, "color_change");
		game->initialized = true;

		game->red = 1.0f;
		game->green = 1.0f;
		game->blue = 1.0f;
	}

	// Update
	if (!paused)
	{
		game->time += delta;
		for (uint32 entity = 0; entity < ENTITY_COUNT; ++entity)
		{
			if (HasComponent(&game->world, entity, PLAYER_MASK))
				PlayerUpdate(&game->world, entity, delta, input);
			if (HasComponent(&game->world, entity, COMPONENT_VELOCITY | COMPONENT_TRANSFORM | COMPONENT_AABB))
				VelocityUpdate(&game->world, entity, delta);
			if (HasComponent(&game->world, entity, COMPONENT_TRANSFORM | COMPONENT_AABB))
				AABBUpdate(&game->world, entity);
		}
		/*
		Vector2 position = game->world.transforms[game->player].position;
		Vector2 target;
		target.x = position.x - (1920.0f / game->camera_scale)*0.5f;
		target.y = position.y - (1080.0f / game->camera_scale)*0.5f;
		game->camera_pos.x += ((game->camera_pos.x - target.x) * delta);
		game->camera_pos.y += ((game->camera_pos.y - target.y) * delta);*/

		if (IsKeyPressed(input, SDL_SCANCODE_F9))
			game->render_aabbs = !game->render_aabbs;

		if (IsKeyPressed(input, SDL_SCANCODE_F1))
			if (game->red == 1) game->red = 0;
			else game->red = 1;

			if (IsKeyPressed(input, SDL_SCANCODE_F2))
				if (game->green == 1) game->green = 0;
				else game->green = 1;

				if (IsKeyPressed(input, SDL_SCANCODE_F3))
					if (game->blue == 1) game->blue = 0;
					else game->blue = 1;

					if (game->camera_pos.x < 0) game->camera_pos.x = 0;
					if (game->camera_pos.y < 0) game->camera_pos.y = 0;
					if (game->camera_pos.x + 1920.0f / game->camera_scale > MAP_W*32.0f + 16.0f)
						game->camera_pos.x = (MAP_W*32.0f + 16.0f) - 1920.0f / game->camera_scale;
					if (game->camera_pos.y + 1080.0f / game->camera_scale > MAP_H*32.0f + 16.0f)
						game->camera_pos.y = (MAP_H*32.0f + 16.0f) - 1080.0f / game->camera_scale;

	}
	// Rendering
	RenderClear(render_context, 32, 20, 41, 255);

	BeginRenderer(render_context, Matrix4_scale(game->camera_scale, game->camera_scale, 1.0f) *
		Matrix4_translate(-game->camera_pos.x, -game->camera_pos.y, 0.0f));
	glUniform3f(game->color_change_loc, game->red, game->green, game->blue);
	
	
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
					Vector2 tile = GetTileFromID(game->map[y][x], &game->sheet, 32);
					RenderTexture(render_context, pos.x, pos.y, 0.0f, &game->sheet, tile.x, tile.y, 32.0f, 32.0f);
				}
			}
		}
	}

	for (uint32 entity = 0; entity < ENTITY_COUNT; ++entity)
	{
		if (HasComponent(&game->world, entity, COMPONENT_SPRITE))
			SpriteRender(&game->world, entity, render_context);
	}
	
	if (game->render_aabbs)
	{
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
	}

	EndRenderer();
	
}