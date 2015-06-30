#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include "std.h"
#include "math.h"
#include <map>

typedef std::map<SDL_Scancode, bool> key_map;
struct InputData
{
	Vector2 mouse_pos;
	key_map keyboard_state;
	key_map prev_keyboard_state;

	bool mb_left;
	bool mb_right;
	bool mb_middle;

	bool mb_left_prev;
	bool mb_right_prev;
	bool mb_middle_prev;
};

bool IsKeyDown(InputData *input, const char *key);
bool IsKeyUp(InputData *input, const char *key);
bool IsKeyPressed(InputData *input, const char *key);
bool IsKeyReleased(InputData *input, const char *key);

enum MouseButton
{
	MB_LEFT,
	MB_RIGHT,
	MB_MIDDLE
};

bool IsMouseDown(InputData *input, MouseButton button);
bool IsMouseUp(InputData *input, MouseButton button);
bool IsMousePressed(InputData *input, MouseButton button);
bool IsMouseReleased(InputData *input, MouseButton button);

#endif // INPUT_H