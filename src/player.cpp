// Header
#include "player.hpp"

// stlib
#include <string>
#include <algorithm>

#include <iostream>
#include <cmath>

//Texture Player::player_texture;

Player::Player(Team team, vec2 position) {
    this->team.assigned = team;
    this->position.pos_x = position.x;
    this->position.pos_y = position.y;
//    this->player_color = {1.f, 1.f, 1.f};
}

//Player::~Player() {}

bool Player::is_texture_loaded(const char *path) {
    // Load shared texture
    if (!player_texture.is_valid()) {
        if (!player_texture.load_from_file(path)) {
            fprintf(stderr, "Failed to load player texture!");
            return false;
        }
    }
    return true;
}

bool Player::init()
{
	switch (this->team.assigned) {
	case Team::PLAYER1:
		is_texture_loaded(textures_path("red_player/CaptureTheCastle_red_player_right.png"));
		break;
	case Team::PLAYER2:
		is_texture_loaded(textures_path("blue_player/CaptureTheCastle_blue_player_right.png"));
		break;
	}

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
	motion.speed = 200.f;

	physics.scale = { 0.3f, 0.3f };
	currDir = { 0, 0, 0, 0, 0 };
	m_is_alive = true;
	m_light_up_countdown_ms = -1.f;

	return true;
}

// Releases all graphics resources
void Player::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

// Called on each frame by World::update()
void Player::update(float ms) {
    float step = motion.speed * (ms / 1000);
    if (m_is_alive) {
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // UPDATE player POSITION HERE BASED ON KEY PRESSED (World::on_key())
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

        if (currDir.down) {
            move({0.f, step});
        }
        if (currDir.up) {
            move({0.f, -step});
        }
        if (currDir.left) {
            move({-step, 0.f});
        }
        if (currDir.right) {
            move({step, 0.f});
        }

    } else {
// If dead we make it face upwards and sink deep down
        move({0.f, step});
    }

    if (m_light_up_countdown_ms > 0.f)
        m_light_up_countdown_ms -=
                ms;
}

void Player::draw(const mat3 &projection) {
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
    transform.translate({position.pos_x, position.pos_y});

    if (currDir.flip) {
        transform.scale({-physics.scale.x, physics.scale.y});
    } else {
        transform.scale(physics.scale);
    }
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    transform.end();

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) 0);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *) sizeof(vec3));

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, player_texture.id);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *) &transform.out);

    // !!! player Color
    float color[] = {1.f, 1.f, 1.f};
//    float color[] = {player_color.x, player_color.y, player_color.z};
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *) &projection);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HERE TO SET THE CORRECTLY LIGHT UP THE player IF HE HAS EATEN RECENTLY
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int light_up = 0;
    glUniform1iv(light_up_uloc, 1, &light_up);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

//// Simple bounding box collision check
//// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
//// if the center point of either object is inside the other's bounding-box-circle. You don't
//// need to try to use this technique.
//bool Player::collides_with(const Turtle& turtle)
//{
//	float dx = motion.position.x - turtle.get_position().x;
//	float dy = motion.position.y - turtle.get_position().y;
//	float d_sq = dx * dx + dy * dy;
//	float other_r = std::max(turtle.get_bounding_box().x, turtle.get_bounding_box().y);
//	float my_r = std::max(physics.scale.x, physics.scale.y);
//	float r = std::max(other_r, my_r);
//	r *= 0.6f;
//	if (d_sq < r * r)
//		return true;
//	return false;
//}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HANDLE player - WALL COLLISIONS HERE
// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
// You will want to write new functions from scratch for checking/handling 
// player - wall collisions.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

vec2 Player::get_position() const {
    return {position.pos_x, position.pos_y};
}

void Player::move(vec2 off) {
    position.pos_x += off.x;
    position.pos_y += off.y;
}

bool Player::is_alive() const {
    return m_is_alive;
}

// Called when the player collides with a turtle
void Player::kill() {
    m_is_alive = false;
}

// Called when the player collides with a fish
void Player::light_up() {
    m_light_up_countdown_ms = 1500.f;
}

// Set direction
void Player::set_direction(int key) {
    switch (key) {
        case GLFW_KEY_DOWN:
            currDir.down = 1;
            break;
        case GLFW_KEY_UP:
            currDir.up = 1;
            break;
        case GLFW_KEY_LEFT:
            currDir.left = 1;
            currDir.flip = 1;
            break;
        case GLFW_KEY_RIGHT:
            currDir.right = 1;
            currDir.flip = 0;
            break;
        default:
            currDir = {0, 0, 0, 0, currDir.flip};
            break;
    }
}

// Set position
void Player::set_position(vec2 pos) {
    position.pos_x = pos.x;
    position.pos_y = pos.y;
}

const Team Player::get_team() {
    return this->team.assigned;
}

vec2 Player::get_bounding_box() {
    return {fabs(physics.scale.x) * player_texture.width, fabs(physics.scale.y) * player_texture.height};
}

bool Player::collides_with_tile(const Tile &tile) {
    float pl = position.pos_x;
    float pr = pl + get_bounding_box().x;
    float pt = position.pos_y;
    float pb = pt + get_bounding_box().y;

    float tt = tile.get_position().y;
    float tb = tt + tile.get_bounding_box().y;
    float tl = tile.get_position().x;
    float tr = tl + tile.get_bounding_box().x;

    bool x_overlap = (pr >= tl && pr <= tr) || (pl <= tr && pl >= tl) || (pr >= tl && pl <= tl);
    bool y_overlap = (pt >= tt && pt <= tb) || (pb <= tb && pb >= tt) || (pb >= tb && pt <= tt);

    return x_overlap && y_overlap;

}

// For debugging purposes
//void Player::change_color(bool colliding) {
//    if (!colliding) {
//        player_color = {1.f, 1.f, 1.f};
//    } else {
//        player_color = {0.f, 0.f, 0.f};
//    }
//}

bool Player::is_left(){
    if (currDir.left == 1 && currDir.right == 0)
        return true;
    return false;
}

bool Player::is_right(){
    if (currDir.left == 0 && currDir.right == 1)
        return true;
    return false;
}

bool Player::is_up(){
    if (currDir.up == 1 && currDir.down == 0)
        return true;
    return false;
}

bool Player::is_down(){
    if (currDir.up == 0 && currDir.down == 1)
        return true;
    return false;
}