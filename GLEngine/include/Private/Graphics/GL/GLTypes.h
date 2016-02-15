#pragma once

#include <stdint.h>

typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef char GLchar;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;

typedef struct __GLsync* GLsync;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#if defined(_MSC_VER) && _MSC_VER < 1400
typedef __int64 GLint64EXT;
typedef unsigned __int64 GLuint64EXT;
#elif defined(_MSC_VER) || defined(__BORLANDC__)
typedef signed long long GLint64EXT;
typedef unsigned long long GLuint64EXT;
#else
typedef int64_t GLint64EXT;
typedef uint64_t GLuint64EXT;
#endif
typedef GLint64EXT  GLint64;
typedef GLuint64EXT GLuint64;

typedef struct
{
	GLuint vertexCount;
	GLuint instanceCount;
	GLuint firstVertex;
	GLuint baseInstance;
} DrawArraysIndirectCommand;

typedef struct
{
	GLuint vertexCount;
	GLuint instanceCount;
	GLuint firstVertex;
	GLuint baseVertex;
	GLuint baseInstance;
} DrawElementsIndirectCommand;