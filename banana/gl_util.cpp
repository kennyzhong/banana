#include "gl_util.h"

internal void CheckShaderError(GLuint shader, GLuint flag, bool is_program, const char *error_message)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (is_program)
		glGetProgramiv(shader, flag, &success);
	else
		glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (is_program)
			glGetProgramInfoLog(shader, sizeof(error), 0, error);
		else
			glGetShaderInfoLog(shader, sizeof(error), 0, error);

		char buffer[1024];

		sprintf(buffer, "%s; %s'", error_message, error);
		Error(buffer);
	}
}

internal GLuint CreateGLShader(const char *text, GLenum type)
{
	GLuint shader = glCreateShader(type);

	if (shader == 0)
		Error("Shader creation failed");

	const GLchar *shaderSourceString = (const GLchar *)text;
	GLint shaderSourceStringLength = (GLint)strlen(text);
	glShaderSource(shader, 1, &shaderSourceString, &shaderSourceStringLength);
	glCompileShader(shader);

	CheckShaderError(shader, GL_COMPILE_STATUS, false, "Shader compilation failed: ");

	return shader;
}

internal char * FileToBuf(char *file)
{
	Sint64 length;
	char *buf;

	SDL_RWops *rw = SDL_RWFromFile(file, "r");
	if (rw != 0)
	{
		length = SDL_RWsize(rw);
		buf = (char *)SDL_malloc(length + 1);
		SDL_RWread(rw, buf, length, 1);
		SDL_RWclose(rw);
		buf[length] = 0;
		return buf;
	}
	return 0;
}


Shader CreateShader(char *filename_vertex, char *filename_fragment)
{
	Shader shader = { 0 };

	char *vertex_source = FileToBuf(filename_vertex);
	char *fragment_source = FileToBuf(filename_fragment);


	shader.program = glCreateProgram();
	shader.vertex = CreateGLShader(vertex_source, GL_VERTEX_SHADER);
	glAttachShader(shader.program, shader.vertex);
	shader.fragment = CreateGLShader(fragment_source, GL_FRAGMENT_SHADER);
	glAttachShader(shader.program, shader.fragment);

	glLinkProgram(shader.program);
	CheckShaderError(shader.program, GL_LINK_STATUS, true, "Program linking failed: ");
	glValidateProgram(shader.program);
	CheckShaderError(shader.program, GL_VALIDATE_STATUS, true, "Program validation failed: ");

	SDL_free(vertex_source);
	SDL_free(fragment_source);

	return shader;
}

void DestroyShader(Shader *shader)
{
	glDetachShader(shader->program, shader->vertex);
	glDetachShader(shader->program, shader->fragment);
	glDeleteProgram(shader->program);
}

void BindShader(Shader *shader)
{
	glUseProgram(shader->program);
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture LoadTexture(char *filename, GLfloat blending)
{
	Texture texture = { 0 };
	int comps;
	unsigned char *image_data = stbi_load(filename, &texture.width, &texture.height, &comps, 4);
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, blending);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, blending);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	
	stbi_image_free(image_data);
	return texture;
}

void UnloadTexture(Texture *texture)
{
	glDeleteTextures(1, &texture->id);
}

#include "math.h"

void InitializeInstancedMap(InstancedMap *map, const void *transforms, const void *tex_offsets,
	Shader *shader, int instance_num)
{
	map->transforms = transforms;
	map->tex_offsets = tex_offsets;
	map->instance_num = instance_num;
	map->shader = shader;
	float v = 0.5f;
	Vector2 vertices[] =
	{
		Vector2(-v, v), Vector2(0.0f, 1.0f),
		Vector2(v, v), Vector2(1.0f, 1.0f),
		Vector2(v, -v), Vector2(1.0f, 0.0f),
		Vector2(-v, -v), Vector2(0.0f, 0.0f)
	};

	glGenVertexArrays(1, &map->vao);
	glBindVertexArray(map->vao);
}

void RenderInstancedMap(InstancedMap *map, Texture *texture, Matrix4 transform)
{
	BindShader(map->shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glBindVertexArray(map->vao);
	glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map->ebo);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, map->instance_num);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}