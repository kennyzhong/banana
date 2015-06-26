#include "ui.h"
#include "input.h"
#include "rendering.h"
#include "world.h"

bool GUIButton(UIContext *ui_context, UIID id, char *text, float x, float y, float w, float h)
{
	bool result = false;
	uint8 r = 255;

	if (Intersects(ui_context->input->mouse_pos, ui_context->input->mouse_pos,
		Vector2(x - w*0.5f, y - h*0.5f), Vector2(x + w*0.5f, y + h*0.5f)))
	{
		if (ui_context->active.index == 0)
		{
			ui_context->hot = id;
		}
		r = 150;
	}

	if (ui_context->active.index == id.index)
	{
		if (IsMouseReleased(ui_context->input, MB_LEFT) && 
			Intersects(ui_context->input->mouse_pos, ui_context->input->mouse_pos,
			Vector2(x - w*0.5f, y - h*0.5f), Vector2(x + w*0.5f, y + h*0.5f)))
		{
			if (ui_context->hot.index == id.index) result = true;
			ui_context->active.index = 0;
		}
	}
	else if (ui_context->hot.index == id.index)
	{
		if (IsMouseDown(ui_context->input, MB_LEFT) && Intersects(ui_context->input->mouse_pos, ui_context->input->mouse_pos,
			Vector2(x - w*0.5f, y - h*0.5f), Vector2(x + w*0.5f, y + h*0.5f))) ui_context->active = id;
	}
		
	
	RenderSquare(ui_context->render_context, x, y, w, h, r, 0, 0, 255);
	RenderString(ui_context->render_context, x, y, text, 5.0f);

	return result;
}