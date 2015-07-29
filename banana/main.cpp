#include <SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include "std.h"
#include "game.h"
#include "input.h"
#include "rendering.h"
#include "editor.h"
#include "ui.h"
#include "voxels.h"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

internal void OutputSystemInfo()
{
	SDL_Log("System Info\n");
	int ram = SDL_GetSystemRAM();
	SDL_Log("Ram: %dMB\n", ram);

	int cpu_count = SDL_GetCPUCount();
	SDL_Log("CPU Count: %d\n", cpu_count);

	const char *platform = SDL_GetPlatform();
	SDL_Log("Platform: %s\n", platform);


	int display_num = SDL_GetNumVideoDisplays();
	SDL_Log("Display Count: %d\n", display_num);

	for (int i = 0; i < display_num; i++)
	{
		SDL_Rect rect;
		SDL_GetDisplayBounds(i, &rect);
		const char *name = SDL_GetDisplayName(i);

		SDL_Log("Display: %s\n", name);
		SDL_Log("-X: %d, ", rect.x);
		SDL_Log("Y: %d, ", rect.y);
		SDL_Log("Width: %d, ", rect.w);
		SDL_Log("Height: %d\n", rect.h);


		for (int j = 0; j < SDL_GetNumDisplayModes(i); j++)
		{
			SDL_DisplayMode mode;
			SDL_GetDisplayMode(i, j, &mode);
			SDL_Log("--Mode: \n");
			SDL_Log("---Width: %d\n", mode.w);
			SDL_Log("---Height: %d\n", mode.h);
			SDL_Log("---Refresh Rate: %d\n", mode.refresh_rate);
		}
	}
}

internal void ToggleFullScreen(SDL_Window *window, WindowData *window_data)
{
	if (window_data->fullscreen)
	{
		window_data->fullscreen = false;
		SDL_SetWindowFullscreen(window, SDL_GetWindowFlags(window) & ~SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	else
	{
		window_data->fullscreen = true;
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
}

global_variable uint64 perfCountFreq;

internal float32 GetSecondsElapsed(uint64 start, uint64 end)
{
	float32 result = ((float32)(end - start) / (float32)perfCountFreq);
	return result;
}

internal uint64 GetWallClock()
{
	return SDL_GetPerformanceCounter();
}

internal void FixViewport(int w, int h, WindowData *window_data)
{
	float32 target_aspect_ratio = (float32)window_data->target_width / (float32)window_data->target_height;
	window_data->width = w;
	window_data->height = h;

	int width = window_data->width;
	int height = (int)(width / target_aspect_ratio + 0.5f);

	if (height > window_data->height)
	{
		// we must letterbox
		height = window_data->height;
		width = (int)(height * target_aspect_ratio + 0.5f);
	}

	window_data->vp_x = (window_data->width / 2) - (width / 2);
	window_data->vp_y = (window_data->height / 2) - (height / 2);
	window_data->vp_width = width;
	window_data->vp_height = height;
	glViewport(window_data->vp_x, window_data->vp_y, width, height);
}

enum Mode
{
	MODE_GAME,
	MODE_EDITOR
};

struct AssetMemory
{
	void *memory;
	uint32 size;
};

int main(int argc, char *args[])
{
#if _WIN32
	// NOTE(nathan): Set the windows schedular granularity to 1ms
	// so that our SDL_Delay() and be more granular

	// SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS); 
	// NOTE(nathan): This seems to give a more consistant result than above
	timeBeginPeriod(1);
#if _DEBUG
	HWND console = GetConsoleWindow();
	if(console != 0)
	MoveWindow(console, -900, 100, 700, 800, TRUE);
#endif
#endif
	SDL_Init(SDL_INIT_EVERYTHING);

	// NOTE(nathan): This also initializes the window_with and window_height
	WindowData window_data = { 0 };
	window_data.target_width = 1920;
	window_data.target_height = 1080;

	FixViewport(1280, 720, &window_data);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

	SDL_Window *window = SDL_CreateWindow(
		"Banana",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_data.width, window_data.height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	// ToggleFullScreen(window);
	SDL_ShowCursor(SDL_DISABLE);
	// OutputSystemInfo();
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) Error("glewInit");

	// Goes stuttery if we turn on vsync
	SDL_GL_SetSwapInterval(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	bool running = true;
	SDL_Event e;

	uint32 gameUpdateHz = 60;
	float32 targetSecondsPerFrame = 1.0f / (float32)gameUpdateHz;

	perfCountFreq = SDL_GetPerformanceFrequency();
	uint64 lastCounter = GetWallClock();

	GameMemory game_memory = { 0 };
	game_memory.size = Megabytes(64);
	game_memory.memory = SDL_malloc(game_memory.size);

	EditorMemory editor_memory = { 0 };
	editor_memory.size = Megabytes(64);
	editor_memory.memory = SDL_malloc(editor_memory.size);

	RenderContext render_context = {};
	render_context.diffuse = CreateShader("assets/shaders/diffuse.vert", "assets/shaders/diffuse.frag");
	InitializeContext(&render_context);

	VoxelRenderContext voxel_render_context = {};
	voxel_render_context.diffuse = CreateShader("assets/shaders/voxels.vert", "assets/shaders/voxels.frag");
	InitializeVoxelContext(&voxel_render_context);

	InputData input = { };
	input.window = window;

	UIContext ui_context = { 0 };
	ui_context.input = &input;
	ui_context.render_context = &render_context;

	Mode mode = MODE_GAME;

	bool game_paused = false;

	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				running = false;
			if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					FixViewport((int)e.window.data1, (int)e.window.data2, &window_data);
				}
			}
			if (e.type == SDL_KEYDOWN)
			{
				SDL_Scancode key = e.key.keysym.scancode;
				input.keyboard_state[key] = true;
				if (key == SDL_SCANCODE_ESCAPE)
					running = false;
				if (key == SDL_SCANCODE_F11)
					ToggleFullScreen(window, &window_data);
				if (key == SDL_SCANCODE_F10)
				{
					if (mode == MODE_GAME)
					{
						game_paused = true;
						mode = MODE_EDITOR;
					}
					else
					{
						mode = MODE_GAME;
						game_paused = false;
					}
				}
			}
			if (e.type == SDL_KEYUP)
			{
				SDL_Scancode key = e.key.keysym.scancode;
				input.keyboard_state[key] = false;
			}
			if (e.type == SDL_MOUSEMOTION)
			{
				input.mouse_pos.x = (((float)window_data.target_width / (float)window_data.vp_width) 
					* ((float)e.motion.x - window_data.vp_x));
				input.mouse_pos.y = (((float)window_data.target_height / (float)window_data.vp_height) 
					* ((float)e.motion.y - window_data.vp_y));
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
					input.mb_left = true;
				if (e.button.button == SDL_BUTTON_RIGHT)
					input.mb_right = true;
				if (e.button.button == SDL_BUTTON_MIDDLE)
					input.mb_middle = true;
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
					input.mb_left = false;
				if (e.button.button == SDL_BUTTON_RIGHT)
					input.mb_right = false;
				if (e.button.button == SDL_BUTTON_MIDDLE)
					input.mb_middle = false;
			}
		}

		float32 delta = 1.0f / (float32)gameUpdateHz;

		GameUpdateAndRender(&game_memory, &input, &render_context, &voxel_render_context, 
			&window_data, game_paused, delta);

		uint64 workCounter = GetWallClock();
		float32 workSecondsElapsed = GetSecondsElapsed(lastCounter, workCounter);
		// Dont include this in frametime
		if (mode == MODE_EDITOR)
			EditorUpdateAndRender(&editor_memory, &input, &render_context, &ui_context, delta);

		float32 secondsElapsedForFrame = workSecondsElapsed;
		if (secondsElapsedForFrame < targetSecondsPerFrame)
		{
			uint32 sleepMS = (uint32)((1000.0f * (targetSecondsPerFrame - secondsElapsedForFrame)) - 1);
			if (sleepMS > 0)
				SDL_Delay(sleepMS);
			while (secondsElapsedForFrame < targetSecondsPerFrame)
			{
				secondsElapsedForFrame = GetSecondsElapsed(lastCounter, GetWallClock());
			}
		}
		else
		{
			// Error("Missed Frame Rate");
		}

		uint64 endCounter = GetWallClock();
		float64 msPerFrame = 1000.0f * GetSecondsElapsed(lastCounter, endCounter);
		lastCounter = endCounter;

		// Debug Render
		BeginRenderer(&render_context);

		std::string s = "FRAMETIME " + std::to_string(msPerFrame);
		RenderString(&render_context, 40.0f, 40.0f, s.c_str(), 0.0f);

		EndRenderer();

		SDL_GL_SwapWindow(window);
		input.prev_keyboard_state = input.keyboard_state;
		input.mb_left_prev = input.mb_left;
		input.mb_right_prev = input.mb_right;
		input.mb_middle_prev = input.mb_middle;
	}
	UnloadContext(&render_context);
	UnloadVoxelContext(&voxel_render_context);
	SDL_free(game_memory.memory);

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return(0);
}