#include "rendering.h"

void InitializeContext(RenderContext *context)
{
	float v = 0.5f;

	float vertices[] = {  
		//	 X	Y		U		V
			-v,	v,		0.0f,	1.0f,		// Front top left		0
			 v, v,		1.0f,	1.0f,		// Front top right		1
			 v, -v,		1.0f,	0.0f,		// Front bottom right	2
			-v, -v,		0.0f,	0.0f		// Front bottom left		3
	};

	glGenBuffers(1, &context->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
	glGenVertexArrays(1, &context->vao);
	glBindVertexArray(context->vao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);
	GLint pos = glGetAttribLocation(context->diffuse.program, "position");
	GLint tex = glGetAttribLocation(context->diffuse.program, "tex_coord");

	glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(pos);

	glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const GLvoid *)(2 * sizeof(float)));
	glEnableVertexAttribArray(tex);

	GLuint elements[] = {
		0, 1, 2,
		0, 2, 3
	};

	glGenBuffers(1, &context->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	context->color_loc = glGetUniformLocation(context->diffuse.program, "color");
	context->world_loc = glGetUniformLocation(context->diffuse.program, "world");
	context->camera_loc = glGetUniformLocation(context->diffuse.program, "camera");
	context->using_tex_loc = glGetUniformLocation(context->diffuse.program, "using_tex");
	context->tex_offset_loc = glGetUniformLocation(context->diffuse.program, "tex_offset");

	// Camera shit
	BindShader(&context->diffuse);
	Matrix4 projection = Matrix4_ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(context->diffuse.program, "projection"),
		1, GL_FALSE, &projection.data[0]);
	glUseProgram(0);

	context->font = LoadTexture("assets/font.png", GL_LINEAR);

}

void UnloadContext(RenderContext *context)
{
	DestroyShader(&context->diffuse);
	glDeleteVertexArrays(1, &context->vao);
	glDeleteBuffers(1, &context->vbo);
	glDeleteBuffers(1, &context->ebo);
	UnloadTexture(&context->font);
}

void RenderClear(RenderContext *context, uint8 r, uint8 g, uint8 b, uint8 a)
{
	glClear(GL_COLOR_BUFFER_BIT);

	BeginRenderer(context);
	// Background letterbox thingy
	RenderSquare(context, 1920.0f * 0.5f, 1080.0f * 0.5f, 1920.0f, 1080.0f,
		r, g, b, a);
	EndRenderer();
}

void BeginRenderer(RenderContext *context, Matrix4 camera)
{
	glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
	glBindVertexArray(context->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
	BindShader(&context->diffuse);
	glUniformMatrix4fv(context->camera_loc, 1, GL_FALSE, &camera.data[0]);
}

void EndRenderer()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void RenderSquare(RenderContext *context, float x, float y, float w, float h,
	uint8 r, uint8 g, uint8 b, uint8 a)
{
	RenderSquare(context, x, y, w, h, 0.0f, r, g, b, a);
}

void RenderSquare(RenderContext *context, float x, float y, float w, float h,
	float rotation, uint8 r, uint8 g, uint8 b, uint8 a, bool outline)
{
	glUniform1i(context->using_tex_loc, 0);
	float32 rr = (1.0f / 255.0f)*(float32)r;
	float32 gg = (1.0f / 255.0f)*(float32)g;
	float32 bb = (1.0f / 255.0f)*(float32)b;
	float32 aa = (1.0f / 255.0f)*(float32)a;

	glUniform4f(context->color_loc, rr, gg, bb, aa);
	Matrix4 world = Matrix4_scale(w, h, 1.0f) * Matrix4_rotate(rotation, 0.0f, 0.0f, 1.0f)
		* Matrix4_translate(x, y, 0.0f);
	glUniformMatrix4fv(context->world_loc, 1, GL_FALSE, &world.data[0]);
	GLenum mode = GL_TRIANGLES;
	if (outline) mode = GL_LINE_LOOP;
	glDrawElements(mode, 6, GL_UNSIGNED_INT, 0);
}

void RenderLine(RenderContext *context, float x1, float y1, float x2, float y2,
	float thickness, uint8 r, uint8 g, uint8 b, uint8 a)
{
	float rotation = atan2f(y2 - y1, x2 - x1) * 180.0f / M_PI;

	float w = sqrtf(powf(x1 - x2, 2) + powf(y1 - y2, 2));
	float x = (x1 + x2) * 0.5f;
	float y = (y1 + y2) * 0.5f;
	RenderSquare(context, x, y, w, thickness, rotation, r, g, b, a);
}

void RenderTexture(RenderContext *context, float x, float y, Texture *texture)
{
	RenderTexture(context, x, y, 0.0f, texture, 0.0f, 0.0f, (float)texture->width, (float)texture->height);
}

void RenderTexture(RenderContext *context, float x, float y, float rotation, Texture  *texture,
	float offset_x, float offset_y, float width, float height,  uint8 r, uint8 g, uint8 b, uint8 a)
{

	float32 rr = (1.0f / 255.0f)*(float32)r;
	float32 gg = (1.0f / 255.0f)*(float32)g;
	float32 bb = (1.0f / 255.0f)*(float32)b;
	float32 aa = (1.0f / 255.0f)*(float32)a;

	glUniform4f(context->color_loc, rr, gg, bb, aa);

	float scale_x = width / (float)texture->width;
	float scale_y = height / (float)texture->height;
	Matrix4 tex_offset = Matrix4_scale(scale_x, scale_y, 1.0f) * 
		Matrix4_translate(
		(1.0f / (float)texture->width)*offset_x, 
		(1.0f / (float)texture->height)*offset_y, 0.0f);
	glUniformMatrix4fv(context->tex_offset_loc, 1, GL_FALSE, &tex_offset.data[0]);
	
	glUniform1i(context->using_tex_loc, 1);
	if (context->bound_texture != texture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->id);
		context->bound_texture = texture;
	}
	Matrix4 world = Matrix4_scale(width, height, 1.0f) * Matrix4_rotate(rotation, 0.0f, 0.0f, 1.0f)
		* Matrix4_translate(x, y, 0.0f);
	glUniformMatrix4fv(context->world_loc, 1, GL_FALSE, &world.data[0]);

	// This isnt causing much lag?? must be the matrix math - got to do simd?? time to learn I guess?
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderString(RenderContext *context, float x, float y, const char *text, float spacing,
	uint8 r, uint8 g, uint8 b, uint8 a)
{
	float char_width = context->font.height;
	float char_height = context->font.height;
	float total_width = (float)strlen(text) * (char_width + spacing);
	for (int i = 0; i < strlen(text); ++i)
	{
		float xx = (x - (total_width * 0.5f)) + i * (char_width + spacing);
		float yy = y - (char_height * 0.5f);
		int font_index = (int)font_text.find(text[i]);
		float offset_x = font_index * char_width;

		if (font_index >= 0)
			RenderTexture(context, xx, yy, 0.0f, &context->font, offset_x, 0.0f, char_width, char_height,
			r, g, b, a);
	}
}

