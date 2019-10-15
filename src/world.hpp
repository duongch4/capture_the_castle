#pragma once

// internal
#include "common.hpp"

#include "tile.hpp"
//#include "background.hpp"


// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <ecs/common_ecs.hpp>
#include <systems/movement_system.hpp>
#include <memory>
#include <systems/player_input_system.hpp>
#include <systems/render_system.hpp>

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

	// Generates a new bandit
	bool spawn_bandit();

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

	// Screen texture
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Background
	//Background m_background;

	// Game entities
    std::vector<Entity> entities;
    std::vector<std::vector<Tile>> m_tiles;

    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<PlayerInputSystem> playerInputSystem;
    std::shared_ptr<SpriteRenderSystem> spriteRenderSystem;

	float m_current_speed;
    float m_next_bandit_spawn;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_real_dist; // default 0..1
};
