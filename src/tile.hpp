#pragma once

#include "common.hpp"
#include "components.hpp"

// Tile class
class Tile
{
	// Shared between all tiles, no need to load one for each instance
	static Texture tile_texture;

public:
	// Use the id to setup the rendering for part of the sprite sheet
	bool init(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width);

	// Releases all the associated resources
	void destroy();

	// Renders the tile
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection);

	// Returns the current tile position
	vec2 get_position()const;

	// Sets the new tile position
	void set_position(vec2 position);

	// Returns the tile's bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

	bool is_wall() const;

    void change_color(bool colliding);

private:
	int tile_id;
    vec3 tile_color;
    bool wall;

    Mesh mesh{};
    Effect effect{};
    Transform transform{};

    mat3 out;

    void set_wall(bool wall);
};