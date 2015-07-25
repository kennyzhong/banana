#include "voxels.h"

void InitializeVoxelContext(VoxelRenderContext *context)
{
	float v = 0.5f;

	float vertices[] = {
		-v, v, -v,				// 0 Front top left
		v, v, -v,				// 1 Front top right
		v, -v, -v,				// 2 Front botom right
		-v, -v, -v,				// 3 Front bottom left
								    
		-v, v, v,				// 4 Back top left
		v, v, v,				// 5 Back top right
		v, -v, v,				// 6 Back botom right
		- v, -v, v,				// 7 Back bottom left
	};

	glGenBuffers(1, &context->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
	glGenVertexArrays(1, &context->vao);
	glBindVertexArray(context->vao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(float), vertices, GL_STATIC_DRAW);
	GLint pos = glGetAttribLocation(context->diffuse.program, "position");

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(pos);

	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 down = Vector3(0.0f, -1.0f, 0.0f);
	Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
	Vector3 left = Vector3(-1.0f, 0.0f, 0.0f);
	Vector3 forward = Vector3(0.0f, 0.0f, -1.0f);
	Vector3 back = Vector3(0.0f, 0.0f, 1.0f);

	Vector3 normals[] = 
	{
		back,back,back,back,back,back,

		right, right, right, right, right, right,

		forward, forward, forward, forward, forward, forward,

		left, left, left, left, left, left,

		up, up, up, up, up, up,

		down, down, down, down, down, down
	};

	glGenBuffers(1, &context->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, context->nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * sizeof(Vector3), normals, GL_STATIC_DRAW);
	GLuint n = glGetAttribLocation(context->diffuse.program, "normal");
	glVertexAttribPointer(n, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vector3), 0);
	glEnableVertexAttribArray(n);


	GLuint elements[] = {
		0, 1, 2,
		0, 2, 3,

		1, 5, 6,
		1, 6, 2,
		
		5, 4, 7,
		5, 7, 6,
		
		4, 0, 3,
		4, 3, 7,
		
		4, 5, 0,
		0, 5, 1,
		
		3, 2, 7,
		7, 2, 6
	};

	context->v_count = sizeof(elements);

	glGenBuffers(1, &context->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	context->color_loc = glGetUniformLocation(context->diffuse.program, "color");
	context->world_loc = glGetUniformLocation(context->diffuse.program, "world");
	context->camera_loc = glGetUniformLocation(context->diffuse.program, "camera");

	// Camera shit
	BindShader(&context->diffuse);
	Matrix4 projection = Matrix4_perspective(90.0f, 1920.0f/1080.0f, 0.01f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(context->diffuse.program, "projection"),
		1, GL_FALSE, &projection.data[0]);
	glUseProgram(0);

}

void UnloadVoxelContext(VoxelRenderContext *context)
{
	DestroyShader(&context->diffuse);
	glDeleteVertexArrays(1, &context->vao);
	glDeleteBuffers(1, &context->vbo);
	glDeleteBuffers(1, &context->ebo);
}

void BeginVoxelRenderer(VoxelRenderContext *context, Matrix4 camera)
{
	glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
	glBindVertexArray(context->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
	BindShader(&context->diffuse);
	glUniformMatrix4fv(context->camera_loc, 1, GL_FALSE, &camera.data[0]);
}

void EndVoxelRenderer()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

void RenderVoxel(VoxelRenderContext *context, Vector3 position, Vector3 scale,
	uint8 r, uint8 g, uint8 b, uint8 a)
{
	RenderVoxel(context, position, scale, Vector3(), r, g, b, a);
}

void RenderVoxel(VoxelRenderContext *context, Vector3 position, Vector3 scale,
	Vector3 rotation, uint8 r, uint8 g, uint8 b, uint8 a, bool outline)
{
	float32 rr = (1.0f / 255.0f)*(float32)r;
	float32 gg = (1.0f / 255.0f)*(float32)g;
	float32 bb = (1.0f / 255.0f)*(float32)b;
	float32 aa = (1.0f / 255.0f)*(float32)a;

	glUniform4f(context->color_loc, rr, gg, bb, aa);
	Matrix4 world = Matrix4_scale(scale.x, scale.y, scale.z) 
		* Matrix4_rotate(rotation.z, 0.0f, 0.0f, 1.0f)
		* Matrix4_rotate(rotation.y, 0.0f, 1.0f, 0.0f)
		* Matrix4_rotate(rotation.x, 1.0f, 0.0f, 0.0f)
		* Matrix4_translate(position.x, position.y, position.z);
	glUniformMatrix4fv(context->world_loc, 1, GL_FALSE, &world.data[0]);
	GLenum mode = GL_TRIANGLES;
	if (outline) mode = GL_LINE_LOOP;
	glDrawElements(mode, context->v_count, GL_UNSIGNED_INT, 0);
}