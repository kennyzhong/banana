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

#endif // GL_UTIL_H