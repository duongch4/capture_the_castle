#pragma once

// internal
#include "common.hpp"

#include "tile.hpp"

#include "player.hpp"
#include "castle.hpp"

#include "background.hpp"


// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

private:

	// Generates a new tile
	bool spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY);

	//// Generates a new turtle
	//bool spawn_turtle();

	//// Generates a new fish
	//bool spawn_fish();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

	// Reset
	void reset();

private:
	// Window handle
	GLFWwindow* m_window;
	float m_screen_scale; // Screen to pixel coordinates scale factor
	vec2 m_screen_size;
	std::vector<vec2> m_start_position;

	// Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Background
	Background m_background;

	// Game entities

	std::vector<std::vector<Tile> > m_tiles;

	std::vector<Player*> players;
	Castle p1_castle;
	Castle p2_castle;

	float m_current_speed;


//	Mix_Music* m_background_music;
//	Mix_Chunk* m_player_dead_sound;
//	Mix_Chunk* m_player_eat_sound;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
