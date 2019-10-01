#pragma once

#include "common.hpp"

// Castle
class Castle : public Entity
{
	static Texture castle_texture;

public:
	// Creates all the associated render resources and default transform
	bool init(bool team, float x, float y);

	// Releases all the associated resources
	void destroy();
	
	// Update castle -- maybe for later when castle may have animation
	// ms represents the number of milliseconds elapsed from the previous update() call
	/*void update(float ms);*/

	// Renders the castle
	// projection is the 2D orthographic projection matrix
	void draw(const mat3& projection) override;

	// Returns the current castle position
	vec2 get_position() const;

	// Sets the new castle position
	void set_position(vec2 position);

	// Returns the castle's bounding box for collision detection, called by collides_with()
	vec2 get_bounding_box() const;

private:
	bool is_red_team; // true if team is red
};