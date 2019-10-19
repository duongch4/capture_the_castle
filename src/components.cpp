//
// Created by Owner on 2019-10-14.
//
#pragma once

#include <sstream>
#include <vector>
#include "components.hpp"

namespace
{
	bool gl_compile_shader(GLuint shader)
	{
		glCompileShader(shader);
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint log_len;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetShaderInfoLog(shader, log_len, &log_len, log.data());
			glDeleteShader(shader);

			fprintf(stderr, "GLSL: %s", log.data());
			return false;
		}

		return true;
	}
}

bool Effect::load_from_file(const char *vs_path, const char *fs_path) {
    gl_flush_errors();

	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);

	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path, fs_path);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);

	// Compiling
	// Shaders already delete if compilation fails
	if (!gl_compile_shader(vertex))
		return false;

	if (!gl_compile_shader(fragment))
	{
		glDeleteShader(vertex);
		return false;
	}

	// Linking
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	{
		GLint is_linked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(program, log_len, &log_len, log.data());

			release();
			fprintf(stderr, "Link error: %s", log.data());
			return false;
		}
	}

	if (gl_has_errors())
	{
		release();
		fprintf(stderr, "OpenGL errors occured while compiling Effect");
		return false;
	}

	return true;
}

void Effect::release()
{
	glDeleteProgram(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

bool Mesh::init(int width, int height) {
    // The position corresponds to the center of the texture.
	float wr = width * 0.4f;
	float hr = height * 0.4f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { 1.f, 1.f, };
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &vao);

	return !gl_has_errors();
}

bool Mesh::init(int textureWidth, int textureHeight, int spriteWidth, int spriteHeight, int spriteIndexX, int spriteIndexY, int spriteGap) {
	// The position corresponds to the center of the texture.
	float wr = textureWidth * 0.5f;
	float hr = textureHeight * 0.5f;

	// Normalize the width & height of a sprite in a sprite sheet (including the gap) on the scale of [0..1]
	float spriteNormalWidth = (float)(spriteWidth + spriteGap) / (float)textureWidth;
	float spriteNormalHeight = (float)(spriteHeight + spriteGap) / (float)textureHeight;

	// Normalize the actual width & height of a sprite (excluding the gap) on the scale of [0..1]
	float spriteNormalActWidth = (float)spriteWidth / (float)textureWidth;
	float spriteNormalActHeight = (float)spriteHeight / (float)textureHeight;

	// Calculate the correct texture coordinate based on the width, height, and the index
	// Texture coordinate are in the range of 0 and 1, and that is why we use the normalized width and height
	// The width and height are then multiplied by the index to get the sprite coordinate on the sprite sheet
	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.01f };
	vertices[0].texcoord = { spriteIndexX * spriteNormalWidth, (spriteIndexY * spriteNormalHeight) + spriteNormalActHeight }; // Bottom left
	vertices[1].position = { +wr, +hr, -0.01f };
	vertices[1].texcoord = { (spriteIndexX * spriteNormalWidth) + spriteNormalActWidth, (spriteIndexY * spriteNormalHeight) + spriteNormalActHeight }; // Bottom right
	vertices[2].position = { +wr, -hr, -0.01f };
	vertices[2].texcoord = { (spriteIndexX * spriteNormalWidth) + spriteNormalActWidth, spriteIndexY * spriteNormalHeight }; // Top Right
	vertices[3].position = { -wr, -hr, -0.01f };
	vertices[3].texcoord = { spriteIndexX * spriteNormalWidth, spriteIndexY * spriteNormalHeight }; // Top Left

	// Counterclockwise as it's the default opengl front winding direction.
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &vao);

	return !gl_has_errors();
}