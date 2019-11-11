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
#include <systems/bandit_ai_system.hpp>
#include <systems/collision_system.hpp>
#include <systems/box_collision_system.hpp>

#include "common.hpp"
#include "components.hpp"
#include "texture_manager.hpp"
#include "tilemap.hpp"
#include <ui/help_window.hpp>
#include <ui/win_window.hpp>

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
	bool initAssetsOpenGL(vec2& screen, const char* title);
	bool loadAudio();
	
	void registerComponents();

	void registerItemBoards(vec2& screen);
	void registerPlayers(std::vector<Entity>& players);
	void registerCastles();
	void registerItemBoard(const Transform& transform, const TeamType& team_type, const char* texture_path);
	Entity registerPlayer(const Transform& transform, const Motion& motion, const TeamType& team_type, const char* texture_path);
	void registerCastle(const Transform& transform, const TeamType& team_type, const char* texture_path);
	
	void registerBanditAiSystem(Entity& player1, Entity& player2);
	void registerBoxCollisionSystem();
	void registerCollisionSystem();
	void registerBanditSpawnSystem();
	void registerSpriteRenderSystem();
	void registerPlayerInputSystem();
	void registerMovementSystem(const vec2& screen);
	
	void renderTilesToScreenTexture();

private:

	// Generates a new tile
	//bool spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY);

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

	// Screen texture
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Game entities
	std::shared_ptr<Tilemap> tilemap;

	// UI
	HelpButton help_btn;
	HelpWindow help_window;
	WinWindow win_window;

    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<PlayerInputSystem> playerInputSystem;
    std::shared_ptr<SpriteRenderSystem> spriteRenderSystem;
    std::shared_ptr<BanditSpawnSystem> banditSpawnSystem;
	std::shared_ptr<BanditAISystem> banditAISystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<BoxCollisionSystem> boxCollisionSystem;

    void winListener(WinEvent* winEvent);

    //World state
	enum WorldState { HELP, WIN, NORMAL };
	WorldState currState;
	CollisionLayer winner;

	// Audio
    Mix_Music* m_background_music;
};
