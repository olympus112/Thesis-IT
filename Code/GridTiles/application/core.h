#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>

#define __cpp_lib_format
#include "util/log.h"

typedef unsigned int GLID;

#include "util/smartPointers.h"

#include "math/vec.h"
#include "math/utils.h"


inline bool glLogCall(const char* func, const char* file, int line) {
	bool response = true;
	unsigned int error = glGetError();
	while (error != GL_NO_ERROR) {
		Log::error("[OpenGL error 0x%x] %s:%d at %s", error, file, line, func);
		error = glGetError();
		response = false;
	}
	return response;
}
#define ASSERT(x) if (!(x)) throw std::logic_error("Assert failed")
#define glCall(x) {while (glGetError() != GL_NO_ERROR); x; ASSERT(glLogCall(#x, __FILE__, __LINE__));}