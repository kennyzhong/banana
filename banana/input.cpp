#include "input.h"

bool IsKeyDown(InputData *input, SDL_Scancode key)
{
	return input->keyboard_state[key];
}

bool IsKeyUp(InputData *input, SDL_Scancode key)
{
	return !input->keyboard_state[key];
}

bool IsKeyPressed(InputData *input, SDL_Scancode key)
{
	if (!input->prev_keyboard_state[key] && IsKeyDown(input, key))
		return true;
	return false;
}

bool IsKeyReleased(InputData *input, SDL_Scancode key)
{
	if (input->prev_keyboard_state[key] && IsKeyUp(input, key))
		return true;
	return false;
}