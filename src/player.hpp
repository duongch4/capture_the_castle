#pragma once

#include "common.hpp"
#include "tile.hpp"
#include <vector>

class Player : public Entity
{

public:
	Player(Team team, vec2 position);

	~Player();

	// Creates all the associated render resources and default transform
	//bool init(Team team, vec2 pos);
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


	// True if the player is alive
	bool is_alive()const;

	// Kills the player, changing its alive state and triggering on death events
	void kill();

	// Sets direction
	void set_direction(int key);

	// Set position
	void set_position(vec2 pos);

	const Team get_team();

	bool collides_with_tile(const Tile& tile);

	void handle_wall_collision();

	vec2 get_bounding_box();
	
//	void change_color(bool colliding);

    bool is_left();

    bool is_right();

    bool is_up();

    bool is_down();


private:
	bool m_is_alive; // True if the player is alive

	//Team m_team;

	Texture player_texture;

//	vec3 player_color;
//
	struct direction { int up, down, left, right, flip; };
	struct collision_response {bool up, down, left, right;};
    collision_response col_res;
	direction currDir;


	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;

	bool is_texture_loaded(const char* path);
};
