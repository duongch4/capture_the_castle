#pragma once

#include "common.hpp"
#include <vector>

class Player : public Entity
{
	static Texture player_texture;

public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Update player position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms);

	// Renders the player
	void draw(const mat3& projection)override;

	// Collision routines for turtles and fish
	/*bool collides_with(const Turtle& turtle);
	bool collides_with(const Fish& fish);*/

	// Returns the current player position
	vec2 get_position() const;

	// Moves the player's position by the specified offset
	void move(vec2 off);

	// Set player rotation in radians
	/*void set_rotation(float radians);*/

	// True if the player is alive
	bool is_alive()const;

	// Kills the player, changing its alive state and triggering on death events
	void kill();

	// Called when the player collides with a fish, starts lighting up the player
	void light_up();

	// Sets direction
	void set_direction(int key);

	// Set position
	void set_position(vec2 pos);

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the player should be lit up
	bool m_is_alive; // True if the player is alive

	enum direction { STAY, UP, DOWN, LEFT, RIGHT } currDir; // Direction of player
	direction prevDir; // Direction Used for Flipping

	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;

	bool Player::is_texture_loaded(const char* path);
};
