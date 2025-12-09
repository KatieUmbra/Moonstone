#pragma once

#define GLFW_INCLUDE_NONE

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Globals.hpp"
#include "glad/glad.h"

namespace gl
{
static void clear_error()
{
	while (glGetError() != GL_NO_ERROR)
		;
}

static auto log_call(const char* function, const char* file, int line) -> bool
{
	while (GLenum error = glGetError())
	{
		gl::error_code = error;
		std::cerr << "[" << __FILE__ << ':' << __LINE__
				  << "][OpenGL][ERROR]: " << error << " at: " << function << ":"
				  << file << ":" << line << '\n';
		return false;
	}
	return true;
}
} // namespace gl

#define __FILENAME__                                                           \
	(std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__)
#ifdef _DEBUG
#define ASSERT(x)                                                              \
	if (!(x))                                                                  \
		__builtin_debugtrap();
#define GL_CALL(x)                                                             \
	gl::clear_error();                                                         \
	x;                                                                         \
	ASSERT(gl::log_call(#x, __FILENAME__, __LINE__))
#else
#define ASSERT(_)
#define GL_CALL(x) x;
#endif
