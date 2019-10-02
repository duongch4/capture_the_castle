// Header
#include "player.hpp"

// internal
#include "turtle.hpp"
#include "fish.hpp"

// stlib
#include <string>
#include <algorithm>

Texture Player::player_texture;

bool Player::is_texture_loaded(const char* path) {
	// Load shared texture
	if (!player_texture.is_valid())
	{
		if (!player_texture.load_from_file(path))
		{
			fprintf(stderr, "Failed to load player texture!");
			return false;
		}
	}
	return true;
}

bool Player::init()
{
	is_texture_loaded(textures_path("blue_player/CaptureTheCastle_blue_player_right.png"));

	// The position corresponds to the center of the texture.
	float wr = player_texture.width * 0.5f;
	float hr = player_texture.height * 0.5f;

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
	motion.position = { 50.f, 100.f };
	motion.radians = 0.f;
	motion.speed = 200.f;

	physics.scale = { 1.f, 1.f };
	m_downDirection = false;
	m_upDirection = false;
	m_leftDirection = false;
	m_rightDirection = false;
	m_is_alive = true;
	m_light_up_countdown_ms = -1.f;

	return true;
}

// Releases all graphics resources
void Player::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

// Called on each frame by World::update()
void Player::update(float ms)
{
	float step = motion.speed * (ms / 1000);
	if (m_is_alive)
	{
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// UPDATE player POSITION HERE BASED ON KEY PRESSED (World::on_key())
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		vec2 moveVec = { 0, 0 };
		if (m_downDirection)
		{
			moveVec.y += step;
		}
		if (m_upDirection)
		{
			moveVec.y -= step;
		}
		if (m_leftDirection)
		{
			moveVec.x -= step;
		}
		if (m_rightDirection)
		{
			moveVec.x += step;
		}
		move(moveVec);

	}
	else
	{
		// If dead we make it face upwards and sink deep down
		set_rotation(3.1415f);
		move({ 0.f, step });
	}

	if (m_light_up_countdown_ms > 0.f)
		m_light_up_countdown_ms -= ms;
}

void Player::draw(const mat3& projection)
{
	transform.begin();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// player TRANSFORMATION CODE HERE

	// see Transformations and Rendering in the specification pdf
	// the following functions are available:
	// translate()
	// rotate()
	// scale()

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// REMOVE THE FOLLOWING LINES BEFORE ADDING ANY TRANSFORMATION CODE
	transform.translate({ motion.position.x, motion.position.y });
	transform.rotate(motion.radians);

	if (currDir == LEFT) {
		transform.scale({ -physics.scale.x, physics.scale.y });
	}
	else {
		transform.scale(physics.scale);
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	transform.end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

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
	glBindTexture(GL_TEXTURE_2D, player_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)& transform.out);

	// !!! player Color
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& projection);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HERE TO SET THE CORRECTLY LIGHT UP THE player IF HE HAS EATEN RECENTLY
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int light_up = 0;
	glUniform1iv(light_up_uloc, 1, &light_up);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

// Simple bounding box collision check
// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool Player::collides_with(const Turtle& turtle)
{
	float dx = motion.position.x - turtle.get_position().x;
	float dy = motion.position.y - turtle.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(turtle.get_bounding_box().x, turtle.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

bool Player::collides_with(const Fish& fish)
{
	float dx = motion.position.x - fish.get_position().x;
	float dy = motion.position.y - fish.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(fish.get_bounding_box().x, fish.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return true;
	return false;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HANDLE player - WALL COLLISIONS HERE
// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
// You will want to write new functions from scratch for checking/handling 
// player - wall collisions.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

vec2 Player::get_position() const
{
	return motion.position;
}

void Player::move(vec2 off)
{
	motion.position.x += off.x;
	motion.position.y += off.y;
}

void Player::set_rotation(float radians)
{
	motion.radians = radians;
}

bool Player::is_alive() const
{
	return m_is_alive;
}

// Called when the player collides with a turtle
void Player::kill()
{
	m_is_alive = false;
}

// Called when the player collides with a fish
void Player::light_up()
{
	m_light_up_countdown_ms = 1500.f;
}

// Set direction
void Player::set_direction(int key) {
	switch (key) {
	case GLFW_KEY_DOWN:
		m_downDirection = true;
		break;
	case GLFW_KEY_UP:
		m_upDirection = true;
		break;
	case GLFW_KEY_LEFT:
		currDir = LEFT;
		m_leftDirection = true;
		break;
	case GLFW_KEY_RIGHT:
		currDir = RIGHT;
		m_rightDirection = true;
		break;
	default:
		currDir = STAY;
		m_downDirection = false;
		m_upDirection = false;
		m_leftDirection = false;
		m_rightDirection = false;
		break;
	}
}

// Set position
void Player::set_position(vec2 position) {
	motion.position = position;
}
