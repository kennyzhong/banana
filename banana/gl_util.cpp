#include "gl_util.h"

GLuint Shader::bound_program = NULL;

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

#include <vector>

Shader CreateShader(char *filename_vertex, char *filename_fragment)
{
	Shader shader = { };

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

	// Cache Uniforms
	GLint num_uniforms = 0;
	glGetProgramInterfaceiv(shader.program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_uniforms);
	const GLenum properties[4] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
	for (int unif = 0; unif < num_uniforms; ++unif)
	{
		GLint values[4];
		glGetProgramResourceiv(shader.program, GL_UNIFORM, unif, 4, properties, 4, 0, values);
		if (values[0] != -1)
			continue;
		std::vector<char> name_data(values[2]);
		glGetProgramResourceName(shader.program, GL_UNIFORM, unif, name_data.size(), 0, &name_data[0]);
		std::string name(name_data.begin(), name_data.end() - 1);
		shader.uniforms[name] = values[3];
	}

	// Cache attributes
	GLint num_attributes = 0;
	glGetProgramiv(shader.program, GL_ACTIVE_ATTRIBUTES, &num_attributes);
	for (int i = 0; i < num_attributes; ++i)
	{
		int length, size;
		GLenum type;
		char buffer[128];
		glGetActiveAttrib(shader.program, i, sizeof(buffer), &length, &size, &type, buffer);
		std::string name(buffer, length);
		shader.attributes[name] = glGetAttribLocation(shader.program, name.c_str());
	}

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

void SetShaderUniform(Shader *shader, char *name, Matrix4 mat)
{
	BindShader(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, name), 1, GL_FALSE, &mat.data[0]);
}

void SetShaderUniform(Shader *shader, char *name, Vector2 vec)
{
	BindShader(shader);
	glUniform2f(shader->uniforms[name], vec.x, vec.y);
	
}

void SetShaderUniform(Shader *shader, char *name, Vector3 vec)
{
	BindShader(shader);
	glUniform3f(shader->uniforms[name], vec.x, vec.y, vec.z);
}

void SetShaderUniform(Shader *shader, char *name, float f)
{
	BindShader(shader);
	glUniform1f(shader->uniforms[name], f);
}

void SetShaderUniform(Shader *shader, char *name, float a, float b, float c, float d)
{
	BindShader(shader);
	glUniform4f(shader->uniforms[name], a, b, c, d);
}

void SetShaderUniform(Shader *shader, char *name, int i)
{
	BindShader(shader);
	glUniform1i(shader->uniforms[name], i);
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

