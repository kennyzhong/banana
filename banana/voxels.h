#ifndef VOXELS_H
#define VOXELS_H

#include <string>
#include "gl_util.h"
#include "math.h"

struct VoxelRenderContext
{
	GLuint vbo;
	GLuint vao;
	GLuint nbo;
	GLuint ebo;
	Shader diffuse;
	int v_count;
};

void InitializeVoxelContext(VoxelRenderContext *context);
void UnloadVoxelContext(VoxelRenderContext *context);

void BeginVoxelRenderer(VoxelRenderContext *context,
	Matrix4 camera = Matrix4_translate(0.0f, 0.0f, 0.0f));
void EndVoxelRenderer();

void RenderVoxel(VoxelRenderContext *context, Vector3 position,  Vector3 scale,
	uint8 r, uint8 g, uint8 b, uint8 a);

void RenderVoxel(VoxelRenderContext *context, Vector3 position, Vector3 scale,
	Quaternion rotation, uint8 r, uint8 g, uint8 b, uint8 a, bool outline = false);

struct Model
{
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint nbo;
	GLuint tbo;
	GLuint cbo;
	int voxel_num;
	Vector3 size;
};

#include "mvload.h"

void BeginModelRenderer(VoxelRenderContext *context, Matrix4 camera, GLenum cull_face = GL_BACK);
void EndModelRenderer();
void InitializeModel(VoxelRenderContext *context, Model *model, MV_Model *mv_model);
void RenderModel(VoxelRenderContext *context, Model *model, Matrix4 transform);
void RenderModel(VoxelRenderContext *context, Model *model, Vector3 position, Vector3 scale,
	Quaternion rotation);

#endif // VOXELS_H