#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <GL/glew.h>
#include <string.h>
#include <map>
#include "std.h"
#include "math.h"


struct Shader
{
	GLuint program;
	GLuint vertex;
	GLuint fragment;
	std::map<std::string, GLint> uniforms;
	std::map<std::string, GLint> attributes;
	local_persist GLuint bound_program;
};

Shader CreateShader(char *filename_vertex, char *filename_fragment);
void DestroyShader(Shader *shader);
void BindShader(Shader *shader);
void SetShaderUniform(Shader *shader, char *name, Matrix4 mat);
void SetShaderUniform(Shader *shader, char *name, Vector2 vec);
void SetShaderUniform(Shader *shader, char *name, Vector3 vec);
void SetShaderUniform(Shader *shader, char *name, float f);
void SetShaderUniform(Shader *shader, char *name, float a, float b, float c, float d);
void SetShaderUniform(Shader *shader, char *name, int i);

struct Texture
{
	GLuint id;
	int width;
	int height;
};

Texture LoadTexture(char *filename, GLfloat blending = GL_NEAREST);
void UnloadTexture(Texture *texture);

#endif // GL_UTIL_H