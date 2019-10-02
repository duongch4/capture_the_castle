// Header
#include "turtle.hpp"

#include <cmath>

Texture Turtle::turtle_texture;

bool Turtle::init() { return false; }

bool Turtle::init(int id, int width, int height)
{
	// Load shared texture
	if (!turtle_texture.is_valid())
	{
		if (!turtle_texture.load_from_file(textures_path("maze.png")))
		{
			fprintf(stderr, "Failed to load turtle texture!");
			return false;
		}
	}

	// The position corresponds to the center of the texture
	float wr = turtle_texture.width * 0.5f;
	float hr = turtle_texture.height * 0.5f;

	// Calculate the texture coordinate based on the id of the sprite
	float tile_width = 1.f / (float)width;
	float tile_height = 1.f / (float)height;
	float tile_act_width = tile_width * (144.f / 164.f); // Each sprite is 144pixel wide and 20 pixel gap between sprite
	float tile_act_height = tile_height * (144.f / 164.f); //

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	// +0.01f to prevent texture bleeding
	vertices[0].texcoord = { ((id - 1) % width) * tile_width + 0.01f, int((id - 1) / width) * tile_height + tile_act_height }; //0.f, 0.5f
	//vertices[0].texcoord = { 0.f, 0.5f };
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { (((id - 1) % width) * tile_width) + tile_act_width + 0.01f, int((id - 1) / width) * tile_height + tile_act_height }; //0.5f, 0.5f
	//vertices[1].texcoord = { 0.5f, 0.5f };
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { (((id - 1) % width) * tile_width) + tile_act_width + 0.01f, int((id - 1) / width) * tile_height }; //0.5f, 0.f
	//vertices[2].texcoord = { 0.5f, 0.f };
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { ((id - 1) % width) * tile_width + 0.01f, int((id  - 1) / width) * tile_height }; //0.f, 0.f
	//vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// Clearing errors
	gl_flush_errors();
	
	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	// Setting initial values
	//position.pos_x = 100.f;
	//position.pos_y = 100.f;					  
	//motion.radians = 0.f;
	motion.speed = 0.f;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	physics.scale = { 0.05f, 0.075f };

	return true;
}

// Releases all graphics resources
void Turtle::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Turtle::update(float ms)
{
	// Move fish along -X based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	//float step = -1.0 * motion.speed * (ms / 1000);
	//position.pos_x += step;
}

void Turtle::draw(const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	transform.begin();
	transform.translate(vec2{position.pos_x, position.pos_y});
	//transform.rotate(motion.radians);
	transform.scale(physics.scale);
	transform.end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// Enabling and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, turtle_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Turtle::get_position()const
{
	return vec2{ position.pos_x, position.pos_y };
}

void Turtle::set_position(vec2 position1)
{
	position.pos_x = position1.x;
	position.pos_y = position1.y;
}

vec2 Turtle::get_bounding_box() const
{
	// Returns the local bounding coordinates scaled by the current size of the turtle 
	// fabs is to avoid negative scale due to the facing direction.
	return { std::fabs(physics.scale.x) * turtle_texture.width, std::fabs(physics.scale.y) * turtle_texture.height };
}