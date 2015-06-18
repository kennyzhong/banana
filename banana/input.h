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
};

bool IsKeyDown(InputData *input, SDL_Scancode key);
bool IsKeyUp(InputData *input, SDL_Scancode key);
bool IsKeyPressed(InputData *input, SDL_Scancode key);
bool IsKeyReleased(InputData *input, SDL_Scancode key);
#endif // INPUT_H