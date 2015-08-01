#include "std.h"
#include "voxels.h"

global_variable float v = 0.5f;

global_variable Vector3 front_top_left = Vector3(-v, v, -v);
global_variable Vector3 front_top_right = Vector3(v, v, -v);
global_variable Vector3 front_bottom_left = Vector3(-v, -v, -v);
global_variable Vector3 front_bottom_right = Vector3(v, -v, -v);

global_variable Vector3 back_top_left = Vector3(-v, v, v);
global_variable Vector3 back_top_right = Vector3(v, v, v);
global_variable Vector3 back_bottom_left = Vector3(-v, -v, v);
global_variable Vector3 back_bottom_right = Vector3(v, -v, v);

global_variable Vector3 vertices[] {
	// Front face
	front_top_left,
	front_top_right,
	front_bottom_right,
	front_bottom_left,
		// Right face
	front_top_right,
	back_top_right,
	back_bottom_right,
	front_bottom_right,
	// Back face
	back_top_right,
	back_top_left,
	back_bottom_left,
	back_bottom_right,
	// Left face
	back_top_left,
	front_top_left,
	front_bottom_left,
	back_bottom_left,
	// Top Face	
	back_top_left,
	back_top_right,
	front_top_right,
	front_top_left,
	// Bottom Face
	front_bottom_left,
	front_bottom_right,
	back_bottom_right,
	back_bottom_left
};

global_variable Vector3 front = Vector3(0.0f, 0.0f, -1.0f);
global_variable Vector3 right = Vector3(1.0f, 0.0f, 0.0f);
global_variable Vector3 back = Vector3(0.0f, 0.0f, 1.0f);
global_variable Vector3 left = Vector3(-1.0f, 0.0f, 0.0f);
global_variable Vector3 top = Vector3(0.0f, 1.0f, 0.0f);
global_variable Vector3 bottom = Vector3(0.0f, -1.0f, 0.0f);

Vector3 normals[] =
{
	// Front face
	front, front, front, front,
	// Right face
	right, right, right, right,
	// Back face
	back, back, back, back,
	// Left face
	left, left, left, left,
	// Top Face	
	top, top, top, top,
	// Bottom Face
	bottom, bottom, bottom, bottom,
};

GLuint elements[] = {
	// Front face
	0, 1, 3, 
	3, 1, 2,
	// right face
	4, 5, 7,
	7, 5, 6,
	// back face
	8, 9, 11,
	11, 9, 10,
	// left face
	12, 13, 15,
	15, 13, 14,
	// top face	
	16, 17, 19,
	19, 17, 18,
	// bottom face
	20, 21, 23,
	23, 21, 22
};


void InitializeVoxelContext(VoxelRenderContext *context)
{

	glGenVertexArrays(1, &context->vao);
	glBindVertexArray(context->vao);

	glGenBuffers(1, &context->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	GLint pos = context->diffuse.attributes["position"];
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
	glEnableVertexAttribArray(pos);

	glGenBuffers(1, &context->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, context->nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * sizeof(Vector3), normals, GL_STATIC_DRAW);
	GLint n = context->diffuse.attributes["normal"];
	glVertexAttribPointer(n, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
	glEnableVertexAttribArray(n);

	context->v_count = sizeof(elements);

	glGenBuffers(1, &context->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// Camera shit
	Matrix4 projection = Matrix4_perspective(90.0f, 1920.0f/1080.0f, 0.01f, 1000.0f);
	SetShaderUniform(&context->diffuse, "projection", projection);

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
	glBindVertexArray(context->vao);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
	BindShader(&context->diffuse);
	SetShaderUniform(&context->diffuse, "camera", camera);
}

void EndVoxelRenderer()
{
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(0);
}

void RenderVoxel(VoxelRenderContext *context, Vector3 position, Vector3 scale,
	uint8 r, uint8 g, uint8 b, uint8 a)
{
	RenderVoxel(context, position, scale, Quaternion(), r, g, b, a);
}

void RenderVoxel(VoxelRenderContext *context, Vector3 position, Vector3 scale,
	Quaternion rotation, uint8 r, uint8 g, uint8 b, uint8 a, bool outline)
{
	float32 rr = (1.0f / 255.0f)*(float32)r;
	float32 gg = (1.0f / 255.0f)*(float32)g;
	float32 bb = (1.0f / 255.0f)*(float32)b;
	float32 aa = (1.0f / 255.0f)*(float32)a;
	SetShaderUniform(&context->diffuse, "instanced", 0);
	SetShaderUniform(&context->diffuse, "color", rr, gg, bb, aa);
	Matrix4 world = Matrix4_scale(scale.x, scale.y, scale.z) 
		* Matrix4_rotate(rotation)
		* Matrix4_translate(position.x, position.y, position.z);
	SetShaderUniform(&context->diffuse, "world", world);
	GLenum mode = GL_TRIANGLES;
	if (outline) mode = GL_LINE_LOOP;
	glDrawElements(mode, context->v_count, GL_UNSIGNED_INT, 0);
}

#include <vector>

void BeginModelRenderer(VoxelRenderContext *context, Matrix4 camera, GLenum cull_face)
{
	SetShaderUniform(&context->diffuse, "camera", camera);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glCullFace(cull_face);
}

void EndModelRenderer()
{
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(0);
}

void InitializeModel(VoxelRenderContext *context, Model *model, MV_Model *mv_model)
{

	std::vector<Matrix4> transforms;
	std::vector<Vector3> colors;
	model->size.x = mv_model->size_x;
	model->size.y = mv_model->size_y;
	model->size.z = mv_model->size_z;

	for (int i = 0; i < mv_model->num_voxels; ++i)
	{
		transforms.push_back(Matrix4_translate(
			(float)mv_model->voxels[i].x - (mv_model->size_x*0.5f),
			(float)mv_model->voxels[i].z - (mv_model->size_z*0.5f),
			(float)mv_model->voxels[i].y - (mv_model->size_y*0.5f)));
		MV_RGBA rgba = mv_model->palette[mv_model->voxels[i].color_index];
		colors.push_back(Vector3(
			rgba.r * (1.0f/255.0f), 
			rgba.g * (1.0f / 255.0f),
			rgba.b * (1.0f / 255.0f)));
	}
	model->voxel_num = mv_model->num_voxels;
	glGenVertexArrays(1, &model->vao);
	glBindVertexArray(model->vao);
	glGenBuffers(1, &model->tbo);
	glBindBuffer(GL_ARRAY_BUFFER, model->tbo);
	glBufferData(GL_ARRAY_BUFFER, transforms.size() * sizeof(Matrix4), &transforms.front(), GL_STATIC_DRAW);

	GLint t = context->diffuse.attributes["world_instanced"];
	for (uint32 i = 0; i < 4; i++)
	{
		glVertexAttribPointer(t + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4),
			(void*)(sizeof(float) * i * 4));
		glEnableVertexAttribArray(t + i);
		glVertexAttribDivisor(t + i, 1);
	}

	glGenBuffers(1, &model->cbo);
	glBindBuffer(GL_ARRAY_BUFFER, model->cbo);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vector3), &colors.front(), GL_STATIC_DRAW);
	GLint c = context->diffuse.attributes["color_instanced"];
	glVertexAttribPointer(c, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
	glEnableVertexAttribArray(c);
	glVertexAttribDivisor(c, 1);

	glGenBuffers(1, &model->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(Vector3), vertices, GL_STATIC_DRAW);
	GLint pos = context->diffuse.attributes["position"];

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
	glEnableVertexAttribArray(pos);

	glGenBuffers(1, &model->nbo);
	glBindBuffer(GL_ARRAY_BUFFER, model->nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * sizeof(Vector3), normals, GL_STATIC_DRAW);
	GLuint n = context->diffuse.attributes["normal"];
	glVertexAttribPointer(n, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vector3), 0);
	glEnableVertexAttribArray(n);




	context->v_count = sizeof(elements);

	glGenBuffers(1, &context->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void RenderModel(VoxelRenderContext *context, Model *model, Matrix4 transform)
{
	//printOpenGLError();
	glBindVertexArray(model->vao);
	SetShaderUniform(&context->diffuse, "instanced", 1);
	SetShaderUniform(&context->diffuse, "world", transform);
	glDrawElementsInstanced(GL_TRIANGLES, context->v_count, GL_UNSIGNED_INT, 0, model->voxel_num);
	SetShaderUniform(&context->diffuse, "instanced", 0);
}

void RenderModel(VoxelRenderContext *context, Model *model, Vector3 position, Vector3 scale,
	Quaternion rotation)
{
	Matrix4 world = Matrix4_scale(scale.x, scale.y, scale.z)
		* Matrix4_rotate(rotation)
		* Matrix4_translate(position.x, position.y, position.z);
	RenderModel(context, model, world);
}