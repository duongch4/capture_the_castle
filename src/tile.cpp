// Header
#include "tile.hpp"

#include <cmath>

Texture Tile::tile_texture;

bool Tile::init() { return false; }

// This function take in an sprite_id (position of the sprite in a sprite sheet), number of horizontal tile, 
// number of vertical tiles, width and height of a single tile. Create a tile with the correct sprite (texture coordinate)
bool Tile::init(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width)
{
	// Load shared texture
	if (!tile_texture.is_valid())
	{
		if (!tile_texture.load_from_file(textures_path("maze_sheet.png")))
		{
			fprintf(stderr, "Failed to load tile texture!");
			return false;
		}
	}

	tile_color = {1.f, 1.f, 1.f};
	// The position corresponds to the center of the texture
	float wr = tile_texture.width * 0.5f;
	float hr = tile_texture.height * 0.5f;

	// Calculate the width & height of a sprite in a sprite sheet with the scale of [0..1]
	float tile_width = (float)(width + gap_width) /  (float)((num_horizontal * width) + ((num_horizontal - 1) * gap_width));
	float tile_height = (float)(width + gap_width) / (float)((num_vertical * width) + ((num_vertical - 1) * gap_width));
	// Calculate the actual width & height of the sprite excluding the gap between each sprite 
	// Each sprite is 144pixel wide and 20 pixel gap between sprite
	float tile_act_width = (float)width / (float)((num_horizontal * width) + ((num_horizontal - 1) * gap_width));
	float tile_act_height = (float)width / (float)((num_vertical * width) + ((num_vertical - 1) * gap_width));

    set_wall(false);
    if (sprite_id != 19){
        set_wall(true);
    }

	// Calculate the texture coordinate based on the id, width, and height of the sprite

	// Texture mapping start from the top left (0,0) to bottom right (1,1)
	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.02f };
	// TODO: +0.01f to prevent texture bleeding for now, fix this later 
	vertices[0].texcoord = { ((sprite_id - 1) % num_horizontal) * tile_width, int((sprite_id - 1) / num_horizontal) * tile_height + tile_act_height }; // Bottom left 
	vertices[1].position = { +wr, +hr, -0.02f };
	vertices[1].texcoord = { (((sprite_id - 1) % num_horizontal) * tile_width) + tile_act_width, int((sprite_id - 1) / num_horizontal) * tile_height + tile_act_height }; // Bottom right
	vertices[2].position = { +wr, -hr, -0.02f };
	vertices[2].texcoord = { (((sprite_id - 1) % num_horizontal) * tile_width) + tile_act_width, int((sprite_id - 1) / num_horizontal) * tile_height }; // Top Right
	vertices[3].position = { -wr, -hr, -0.02f };
	vertices[3].texcoord = { ((sprite_id - 1) % num_horizontal) * tile_width, int((sprite_id  - 1) / num_horizontal) * tile_height }; // Top Left

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
	// To set a default position, rotation and speed, uncomment the section below
	//position.pos_x = 100.f;
	//position.pos_y = 100.f;					  
	//motion.radians = 0.f;
	//motion.speed = 0.f;

	// Setting initial values, scale is negative to make it face the opposite way
	// 1.0 would be as big as the original texture.
	// To make a square, scale the x and y based on the width:height ratio of the sprite sheet 
	physics.scale = { 0.048f, 0.072f };

	return true;
}

// Releases all graphics resources
void Tile::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

void Tile::update(float ms)
{
	// Do nothing for now, but if we ever wanted to animate the tile, this can be used.
}

void Tile::draw(const mat3& projection)
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
	glBindTexture(GL_TEXTURE_2D, tile_texture.id);

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
//	float color[] = { 1.f, 1.f, 1.f };
    float color[] = { tile_color.x, tile_color.y, tile_color.z}; //For collision debugging purposes
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Drawing!
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

vec2 Tile::get_position()const
{
	return vec2{ position.pos_x, position.pos_y };
}

void Tile::set_position(vec2 position1)
{
	position.pos_x = position1.x;
	position.pos_y = position1.y;
}

vec2 Tile::get_bounding_box() const
{
	// Returns the local bounding coordinates scaled by the current size of the tile 
	// fabs is to avoid negative scale due to the facing direction.
	return { std::fabs(physics.scale.x) * tile_texture.width * 0.75f, std::fabs(physics.scale.y) * tile_texture.height * 0.75f};
}

bool Tile::is_wall() const {
    return this->wall;
}

void Tile::set_wall(bool wall) {
    this->wall = wall;
}

void Tile::change_color(bool colliding) {
    if (!colliding) {
        tile_color = {1.f, 1.f, 1.f};
    } else {
        tile_color = {1.f, 0.f, 0.f};
    }
}