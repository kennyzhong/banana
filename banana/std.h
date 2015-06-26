#ifndef STD_H
#define STD_H

#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <string>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

#include <string>
internal void Error(std::string error, bool sdl_err = false)
{
	std::string s;
	if (sdl_err) s = error + SDL_GetError();
	else s = error;
	SDL_ShowSimpleMessageBox(0, "Error!", s.c_str(), 0);
}



inline static int printOglError(char *file, int line) {

	GLenum glErr;
	int    retCode = 0;

	glErr = glGetError();
	if (glErr != GL_NO_ERROR) {
		printf("glError in file %s @ line %d: %s\n",
			file, line, gluErrorString(glErr));
		retCode = 1;
	}

	return retCode;
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

/*
#include <algorithm>

template<typename T, size_t size>
int array_find(const T (&a)[size], const T & c)
{
	const T *found = std::find(&a[0], &a[size], c);
	return found == &a[size] ? -1 : found - a;
}
*/
#endif // STD_H