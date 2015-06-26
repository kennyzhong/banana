#ifndef UI_H
#define UI_H

#include "std.h"

struct UIID
{
	int owner;
	int item;
	int index;
};

struct InputData;
struct RenderContext;

struct UIContext
{
	UIID hot;
	UIID active;

	InputData *input;
	RenderContext *render_context;
};

bool GUIButton(UIContext *ui_context, UIID id, char *text, float x, float y, float w, float h);

#endif // UI_H