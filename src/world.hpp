#pragma once

// internal
#include "common.hpp"

#include "tilemap.hpp"
#include "ui/helpbtn.hpp"


// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <memory>
#include <SDL.h>
#include <SDL_mixer.h>

// internal
#include <ecs/common_ecs.hpp>
#include <ecs/ecs_manager.hpp>

#include <systems/movement_system.hpp>
#include <systems/player_input_system.hpp>
#include <systems/render_system.hpp>
#include <systems/bandit_spawn_system.hpp>
#include <systems/item_spawn_system.hpp>

#include <systems/bandit_ai_system.hpp>
#include <systems/collision_system.hpp>
#include <systems/box_collision_system.hpp>
#include <systems/soldier_ai_system.hpp>

#include "common.hpp"
#include "components.hpp"
#include "texture_manager.hpp"
#include "tilemap.hpp"
#include <ui/help_window.hpp>
#include <ui/win_window.hpp>
#include <states/state.hpp>

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

    vec2 get_screen_size();

    float get_screen_scale();

    void get_buffer_size(int &w, int &h);

    mat3 get_projection_2d();

    void set_window_closed();

    bool set_state(State* state);

    void create_texture_from_window(Texture& tex);

    // Should the game be over ?
	bool is_over()const;

private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
    void on_mouse_click(GLFWwindow *pWwindow, int button, int action, int mods);

    // Reset
	void reset();

private:
	// Window handle
	GLFWwindow* m_window;
	float m_screen_scale; // Screen to pixel coordinates scale factor
	vec2 m_screen_size;
	mat3 projection_2D;

  State* m_state;
};
