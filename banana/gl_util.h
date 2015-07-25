#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <GL/glew.h>
#include <string.h>
#include "std.h"

struct Shader
{
	GLuint program;
	GLuint vertex;
	GLuint fragment;
};

Shader CreateShader(char *filename_vertex, char *filename_fragment);
void DestroyShader(Shader *shader);
void BindShader(Shader *shader);

struct Texture
{
	GLuint id;
	int width;
	int height;
};

Texture LoadTexture(char *filename, GLfloat blending = GL_NEAREST);
void UnloadTexture(Texture *texture);

struct InstancedMap
{
	const void *transforms;
	const void *tex_offsets;
	int instance_num = 0;
	GLuint vbo;
	GLuint vao;
	GLuint ebo;
	GLuint tbo;
	GLuint tobo;
	Shader *shader;
};

#include "math.h"

void InitializeInstancedMap(InstancedMap *map, const void *transforms, const void *tex_offsets, 
	Shader *shader, int instance_num);
void RenderInstancedMap(InstancedMap *map, Texture *texture,
	Matrix4 transform = Matrix4_translate(0.0f, 0.0f, 0.0f));

#endif // GL_UTIL_H