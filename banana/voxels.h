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
	GLint color_loc;
	GLint camera_loc;
	GLint world_loc;
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
	Vector3 rotation, uint8 r, uint8 g, uint8 b, uint8 a, bool outline = false);

#endif // VOXELS_H