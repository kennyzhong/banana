#include "editor.h"
#include "input.h"
#include "rendering.h"
#include "ui.h"

void EditorUpdateAndRender(EditorMemory *editor_memory, InputData *input,
	RenderContext *render_context, UIContext *ui_context, float delta)
{
	EditorState *editor = (EditorState *)editor_memory;
	if (!editor->initialized)
	{
		// Initialization

		editor->initialized = true;
	}


	BeginRenderer(render_context);

	if (GUIButton(ui_context, { 0, 0, 1 }, "WHITE", 400.0f, 400.0f, 300.0f, 100.0f))
	{
	}

	if (GUIButton(ui_context, { 0, 0, 2 }, "RED", 800.0f, 800.0f, 300.0f, 100.0f))
	{
	}

	//SDL_Log("Hot: %d, Active: %d", ui_context->hot.index, ui_context->active.index);

	EndRenderer();

}