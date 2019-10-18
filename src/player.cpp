// Header
#include "player.hpp"

// stlib
#include <string>
#include <algorithm>

#include <iostream>
#include <cmath>

Player::Player(Team team, vec2 position) {
    this->team.assigned = team;
    this->position.pos_x = position.x;
    this->position.pos_y = position.y;
	spriteNum = {0, 0 };
    this->stuck = false;
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

bool Player::set_sprite() {
	// The position corresponds to the center of the texture.
	//std::cout << spriteNum.x << std::endl;

	float wr = spriteSize.x * spriteNum.x + 0.5f * spriteSize.x;
	float hr = spriteSize.y * spriteNum.y + 0.5f * spriteSize.y;

	TexturedVertex vertices[4];
	vertices[0].position = { wr - spriteSize.x, hr + spriteSize.y, -0.01f };
	vertices[0].texcoord = { spriteNum.x / 7, (spriteNum.y + 1) / 5 };
	vertices[1].position = { wr + spriteSize.x, hr + spriteSize.y, -0.01f };
	vertices[1].texcoord = { (spriteNum.x + 1) / 7, (spriteNum.y + 1) / 5, };
	vertices[2].position = { wr + spriteSize.x, hr - spriteSize.y , -0.01f };
	vertices[2].texcoord = { (spriteNum.x + 1) / 7, spriteNum.y / 5 };
	vertices[3].position = { wr - spriteSize.x , hr - spriteSize.y, -0.01f };
	vertices[3].texcoord = { spriteNum.x / 7, spriteNum.y / 5 };

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

	
	return true;
}

bool Player::init()
{
	switch (this->team.assigned) {
	case Team::PLAYER1:
		is_texture_loaded(textures_path("red_king_sprite_sheet.png"));
		break;
	case Team::PLAYER2:
		is_texture_loaded(textures_path("blue_king_sprite_sheet.png"));
		break;
	}
	

	spriteSize.x = player_texture.width / 7.0f;
	spriteSize.y = player_texture.height / 5.0f;

	// Setting initial values
	motion.speed = 200.f;
	//std::cout << "size x: " << spriteSize.x << "   size y: " << spriteSize.y << std::endl;
	moving = false;
	physics.scale = { 0.3f, 0.3f };
	m_is_alive = true;
	currDir = { 0, 0, 0, 0};
	return set_sprite();
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
		
        if (currDir.down) {
				spriteNum.y = 4;
			move({ 0.f, step });
        }
        if (currDir.up) {
				spriteNum.y = 1;
			move({ 0.f, -step });

        }
        if (currDir.left) {
            move({-step, 0.f});
				spriteNum.y = 2;
        }
        if (currDir.right) {
            move({step, 0.f});
				spriteNum.y = 3;
        }
		if (moving && !stuck)
		{
			if (spriteNum.x < 6)
				spriteNum.x++;
			else
			{
				spriteNum.x = 0;
			}
			set_sprite();
		}
    }
}

void Player::draw(const mat3 &projection) {
	//std::cout << position.pos_x << "  " << position.pos_y << std::endl;
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
	transform.translate({ -spriteSize.x * (spriteNum.x)*0.3f - 17, -spriteSize.y * (spriteNum.y)*0.3f - 18 + (spriteNum.y*0.4f)});
    transform.translate({position.pos_x, position.pos_y});

    //if (currDir.flip) {
    //    transform.scale({-physics.scale.x, physics.scale.y});
    //} else {
    //   
    //}

	transform.scale(physics.scale);
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

// Set direction
void Player::set_direction(int key) {
    switch (key) {
        case GLFW_KEY_DOWN:
            currDir.down = 1;
			moving = true;
            break;
        case GLFW_KEY_UP:
            currDir.up = 1;
			moving = true;
            break;
        case GLFW_KEY_LEFT:
            currDir.left = 1;
			moving = true;
            break;
        case GLFW_KEY_RIGHT:
            currDir.right = 1;
			moving = true;
            break;
        default:
            currDir = {0, 0, 0, 0};
			moving = false;
			//std::cout << "default" << std::endl;
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
    return {fabs(physics.scale.x) * spriteSize.x * 1.15f, fabs(physics.scale.y) * spriteSize.y * 1.15f};
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

    col_res.left = (pr >= tl && pr <= tr); //approach from left
    col_res.right = (pl <= tr && pl >= tl); //approach from right
    bool x_over = (pr >= tl && pl <= tl); //overlap

    col_res.down = (pt >= tt && pt <= tb); // approach from bottom
    col_res.up = (pb <= tb && pb >= tt); //approach from top
    bool y_over =  (pb >= tb && pt <= tt); //overlap

    bool x_overlap = col_res.left || col_res.right || x_over;
    bool y_overlap = col_res.down || col_res.up || y_over;

    return x_overlap && y_overlap;
}

void Player::handle_wall_collision(const Tile &tile) {
    // Move player away from the wall by 5.f;
        if (is_left()) {
            position.pos_x -= 10.f;
        } else if (is_right()) {
            position.pos_x += 10.f;
        }

        if (collides_with_tile(tile)) {
			moving = false;
            if (is_up()) {
                position.pos_y -= 10.f;
            } else if (is_down()) {
                position.pos_y += 10.f;
            }
            if (collides_with_tile(tile)) {
                if (is_left()) {
                    position.pos_x -= 10.f;
                } else if (is_right()) {
                    position.pos_x += 10.f;
                }
            }
        }
        currDir = {0, 0, 0, 0};
        set_stuck(true);

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
    return col_res.left;
}

bool Player::is_right(){
    return col_res.right;
}

bool Player::is_up(){
    return col_res.up;
}

bool Player::is_down(){
    return col_res.down;
}

bool Player::is_stuck() {
    return stuck;
}

void Player::set_stuck(bool stuck) {
    this->stuck = stuck;
}