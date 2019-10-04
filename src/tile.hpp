#pragma once

#include "common.hpp"

// Tile class
class Tile : public Entity
{
	// Shared between all tiles, no need to load one for each instance
	static Texture tile_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();
	// Use the id to setup the rendering for part of the sprite sheet
	bool init(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width);

	// Releases all the associated resources
	void destroy();

	// Update tile due to current
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the tile
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	// Returns the current tile position
	vec2 get_position()const;

	// Sets the new tile position
	void set_position(vec2 position);

	// Returns the tile's bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;
};