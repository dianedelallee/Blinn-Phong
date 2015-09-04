#pragma once

#include "GLCommon.hpp"

struct ShaderGLSL
{
	enum ShaderType
	{
		VERTEX_SHADER = 1,
		FRAGMENT_SHADER = 2,
		GEOMETRY_SHADER = 4
	};
	GLuint program;
};

int compile_and_link_shader(ShaderGLSL & shader, int typeMask, const char * sourceBuffer, int bufferSize);
int destroy_shader(ShaderGLSL & shader);
int load_shader_from_file(ShaderGLSL & shader, const char * path, int typemask);
