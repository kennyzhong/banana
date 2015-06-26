#ifndef EDITOR_H
#define EDITOR_H

#include "std.h"

struct InputData;
struct RenderContext;
struct UIContext;

struct EditorMemory
{
	void *memory;
	uint64 size;
};

struct EditorState
{
	bool32 initialized;
};

void EditorUpdateAndRender(EditorMemory *editor_memory, InputData *input,
	RenderContext *render_context, UIContext *ui_context, float delta);

#endif // EDITOR_H