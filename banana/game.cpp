#include "game.h"
#include "input.h"
#include "rendering.h"

void GameUpdateAndRender(GameMemory *game_memory, InputData *input, RenderContext *render_context, float delta)
{
	GameState *game = (GameState *)game_memory->memory;
	if (!game->initialized)
	{
		// Initailzation
		game->texture = LoadTexture("assets/atlas.png");
		game->anim_x = 0.0f;
		game->initialized = true;
	}

	// Update
	game->time += delta;
	int fps = 6;
	if (game->time > (1.0f / (float)fps))
	{
		game->anim_x += 20;
		game->time = 0.0f;
	}
	if (game->anim_x >= 20 * 6)
		game->anim_x = 0.0f;

	// Rendering
	glClear(GL_COLOR_BUFFER_BIT);

	BeginRenderer(render_context, Matrix4_scale(1.0f, 1.0f, 1.0f));
	// Background letterbox thingy
	RenderSquare(render_context, 1920.0f * 0.5f, 1080.0f * 0.5f, 1920.0f, 1080.0f,
		135, 123, 185, 255);

	//RenderTexture(render_context, game->texture.width*0.5f, game->texture.height*0.5f, &game->texture);
	RenderTexture(render_context, 100.0f, 100.0f, 0.0f,
		&game->texture, game->anim_x, 28.0f, 20.0f, 26.0f);

	EndRenderer();
	
}