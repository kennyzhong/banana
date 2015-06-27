#ifndef RENDERING_H
#define RENDERING_H

#include <string>
#include "gl_util.h"
#include "math.h"

global_variable std::string font_text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890.";

struct RenderContext
{
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	GLint color_loc;
	GLint camera_loc;
	GLint world_loc;
	GLint using_tex_loc;
	GLint tex_offset_loc;
	Shader diffuse;
	Texture *bound_texture;
	Texture font;
};

void InitializeContext(RenderContext *context);
void UnloadContext(RenderContext *context);

void RenderClear(RenderContext *context, uint8 r, uint8 g, uint8 b, uint8 a);

void BeginRenderer(RenderContext *context, 
	Matrix4 camera = Matrix4_translate(0.0f, 0.0f, 0.0f));
void EndRenderer();

void RenderSquare(RenderContext *context, float x, float y, float w, float h,
	uint8 r, uint8 g, uint8 b, uint8 a);

void RenderSquare(RenderContext *context, float x, float y, float w, float h,
	float rotation, uint8 r, uint8 g, uint8 b, uint8 a, bool outline = false);

void RenderLine(RenderContext *context, float x1, float y1, float x2, float y2,
	float thickness, uint8 r, uint8 g, uint8 b, uint8 a);

void RenderTexture(RenderContext *context, float x, float y, Texture *texture);
void RenderTexture(RenderContext *context, float x, float y, float rotation, 
	Texture  *texture, float offset_x, float offset_y, float width, float height,
	uint8 r = 255, uint8 g = 255, uint8 b = 255, uint8 a = 255);

void RenderString(RenderContext *context, float x, float y, const char *text, float spacing,
	uint8 r = 255, uint8 g = 255, uint8 b = 255, uint8 a = 255);

#endif // RENDERING_H