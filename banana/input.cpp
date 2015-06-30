#include "input.h"

bool IsKeyDown(InputData *input, const char *key)
{
	return input->keyboard_state[SDL_GetScancodeFromName(key)];
}

bool IsKeyUp(InputData *input, const char *key)
{
	return !input->keyboard_state[SDL_GetScancodeFromName(key)];
}

bool IsKeyPressed(InputData *input, const char *key)
{
	if (!input->prev_keyboard_state[SDL_GetScancodeFromName(key)] && IsKeyDown(input, key))
		return true;
	return false;
}

bool IsKeyReleased(InputData *input, const char *key)
{
	if (input->prev_keyboard_state[SDL_GetScancodeFromName(key)] && IsKeyUp(input, key))
		return true;
	return false;
}

bool IsMouseDown(InputData *input, MouseButton button)
{
	if (button == MB_LEFT)
		return input->mb_left;
	if (button == MB_RIGHT)
		return input->mb_right;
	if (button == MB_MIDDLE)
		return input->mb_middle;
	return false;
}

bool IsMouseUp(InputData *input, MouseButton button)
{
	return !IsMouseDown(input, button);
}

bool IsMousePressed(InputData *input, MouseButton button)
{
	if (button == MB_LEFT)
		if (!input->mb_left_prev && input->mb_left)
			return true;
	if (button == MB_RIGHT)
		if (!input->mb_right_prev && input->mb_right)
			return true;
	if (button == MB_MIDDLE)
		if (!input->mb_middle_prev && input->mb_middle)
			return true;
	return false;
}
bool IsMouseReleased(InputData *input, MouseButton button)
{
	if (button == MB_LEFT)
		if (input->mb_left_prev && !input->mb_left)
			return true;
	if (button == MB_RIGHT)
		if (input->mb_right_prev && !input->mb_right)
			return true;
	if (button == MB_MIDDLE)
		if (input->mb_middle_prev && !input->mb_middle)
			return true;
	return false;
}