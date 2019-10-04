// Header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>

// Same as static in c, local to compilation unit
namespace
{
	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World()
{
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{

}

// World initialization
bool World::init(vec2 screen)
{
	m_screen_size = screen;
	
	m_start_position.push_back({ 150.f, m_screen_size.y / 2 + 130.f });
	m_start_position.push_back({ m_screen_size.x - 150.f, m_screen_size.y / 2 + 130.f });
	
	players.push_back(new Player(Team::PLAYER1, m_start_position[0]));
	players.push_back(new Player(Team::PLAYER2, m_start_position[1]));

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "Capture the Castle", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	// Create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int fb_width, fb_height;
	glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
	m_screen_scale = static_cast<float>(fb_width) / screen.x;

	// Initialize the screen texture
	m_screen_tex.create_from_screen(m_window);

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	// TODO: Uncomment and modify to add background music
//	m_background_music = Mix_LoadMUS(audio_path("music.wav"));
//	m_player_dead_sound = Mix_LoadWAV(audio_path("player_dead.wav"));
//	m_player_eat_sound = Mix_LoadWAV(audio_path("player_eat.wav"));
//
//	if (m_background_music == nullptr || m_player_dead_sound == nullptr || m_player_eat_sound == nullptr)
//	{
//		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
//			audio_path("music.wav"),
//			audio_path("player_dead.wav"),
//			audio_path("player_eat.wav"));
//		return false;
//	}
//
//	// Playing background music indefinitely
//	Mix_PlayMusic(m_background_music, -1);
//
//	fprintf(stderr, "Loaded music\n");

	// Hardcoded maze data, created using Tiled 
	// Each number represent the id of a tile 
	// Id is the position of a sprite in a sprite sheet starting from left to right, top to bottom 
	int data[] = {
		19, 19, 19, 19, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 9, 9, 9, 9, 9, 9, 19, 19, 19, 19, 9, 9, 9, 9, 9, 9, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 9, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 9, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 9, 9, 9, 9, 9, 9, 9, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 9, 19, 19, 19, 19,
		19, 19, 19, 19, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 19, 19, 19, 19
	};

	// TODO: Refactor this later to move it into a TileMap class
	// Create all the tiles based on the maze data we defined above
	for (int j = 0; j < 19; j++) {
		m_tiles.emplace_back(std::vector<Tile>(30));
		for (int i = 0; i < 30; i++) {
			int iter = j * 30 + i;
			// First parameter is the id of the tile, second parameter is the number of tile horizontally in the sprite sheet
			// Third parameter is the number of tile vertically in the sprite sheet.
			if (!spawn_tile(data[iter], 6, 4, 68, 20, i, j)) {
				return false;
			}

			Tile& new_tile = m_tiles[j][i];

			// Setting the tile initial position
			new_tile.set_position({ i * 46.f + 23.f, j * 43.f + 19.f });
		}
	}

	// TODO: CALL INIT ON ALL GAME ENTITIES

	for (auto player : players) {
		player->init();
	}

	m_background.init();
	p1_castle.init(Team::PLAYER1, 150.f, screen.y / 2);
	p2_castle.init(Team::PLAYER2, screen.x - 150.f, screen.y / 2);

	p1_board = new ItemBoard(Team::PLAYER1, { 175.f, 53.f });
	p2_board = new ItemBoard(Team::PLAYER2, { screen.x - 175.f, 53.f });

	p1_board->init();
	p2_board->init();

	return true;
}

// Releases all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	// TODO: MIX_FREEAUDIO AND MIX_FREECHUNK ON ALL AUDIOS
	Mix_CloseAudio();

	// TODO: DESTROY ALL GAME ENTITIES
	// If we move to the next level, destroy all the tiles 
	for (auto& vector : m_tiles) {
		for (auto& tile : vector) {
			tile.destroy();
		}
	}
		
	for (auto& vector : m_tiles) {
		vector.clear();
	}

	m_tiles.clear();

	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w / m_screen_scale, (float)h / m_screen_scale };

	// TODO: COLLISION DETECTIONS

	// TODO: SPAWN GAME ENTITIES

	// Update each of tiles, the update function is empty for now
	// Can be used in the future to animate the tile
	for (auto& vector : m_tiles) {
		for (auto& tile : vector) {
			tile.update(elapsed_ms);
		}
	}

	// Player update
	for (auto player : players) {
		if (player->is_alive()) {
			const float offset_x = 100.f;
			const float offset_y = 80.f;
			if (player->get_position().x > (screen.x - offset_x)) {
				player->set_position({ screen.x - offset_x, player->get_position().y });
			}
			if (player->get_position().x < (0 + offset_x)) {
				player->set_position({ 0 + offset_x, player->get_position().y });
			}
			if (player->get_position().y > (screen.y - offset_y)) {
				player->set_position({ player->get_position().x, screen.y - offset_y });
			}
			if (player->get_position().y < (0 + offset_y)) {
				player->set_position({ player->get_position().x, 0 + offset_y });
			}
		}
		player->update(elapsed_ms);
	}

	return true;
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	/////////////////////////////////////
	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.3f, 0.3f, 0.8f };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Fake projection matrix, scales with respect to window coordinates
	// PS: 1.f / w in [1][1] is correct.. do you know why ? (:
	float left = 0.f;// *-0.5;
	float top = 0.f;// (float)h * -0.5;
	float right = (float)w / m_screen_scale;// *0.5;
	float bottom = (float)h / m_screen_scale;// *0.5;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	// TODO: DRAW GAME ENTITIES USING projection_2D
	//p1_castle.draw(projection_2D);
	//p2_castle.draw(projection_2D);

	//for (auto player : players) {
	//	player->draw(projection_2D);
	//}

	/////////////////////
	// Truely render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0.43f, 0.92f, 0.51f, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);


	// Background
	//m_background.draw(projection_2D);

	// Render all the tiles we have 
	for (auto& vector : m_tiles) {
		for (auto& tile : vector) {
			tile.draw(projection_2D);
		}
	}

	p1_castle.draw(projection_2D);
	p2_castle.draw(projection_2D);

	for (auto player : players) {
		player->draw(projection_2D);
	}

	p1_board->draw(projection_2D);
	p2_board->draw(projection_2D);


	//////////////////
	// Presenting
	glfwSwapBuffers(m_window);

}

// Should the game be over ?
bool World::is_over() const
{
	return glfwWindowShouldClose(m_window);
}

// Creates a new tile and if successfull adds it to the list of tile
bool World::spawn_tile(int sprite_id, int num_horizontal, int num_vertical, int width, int gap_width, int gridX, int gridY)
{
	Tile tile;
	if (tile.init(sprite_id, num_horizontal, num_vertical, width, gap_width))
	{
		m_tiles[gridY].emplace(m_tiles[gridY].begin() + gridX, tile);
		return true;
	}
	fprintf(stderr, "Failed to spawn tile");
	return false;
}
// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// TODO: HANDLE KEY INPUTS
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE SALMON MOVEMENT HERE
	// key is of 'type' GLFW_KEY_
	// action can be GLFW_PRESS GLFW_RELEASE GLFW_REPEAT
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	if (action == GLFW_RELEASE) {
		players[0]->set_direction(GLFW_KEY_S);
		players[1]->set_direction(GLFW_KEY_S);
	}

	if (
		action == GLFW_PRESS &&
		(
			key == GLFW_KEY_DOWN || key == GLFW_KEY_UP ||
			key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT
			)
		)
	{
		players[1]->set_direction(key);
	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_S:
			players[0]->set_direction(GLFW_KEY_DOWN);
			break;
		case GLFW_KEY_W:
			players[0]->set_direction(GLFW_KEY_UP);
			break;
		case GLFW_KEY_D:
			players[0]->set_direction(GLFW_KEY_RIGHT);
			break;
		case GLFW_KEY_A:
			players[0]->set_direction(GLFW_KEY_LEFT);
			break;
		default:
			break;
		}
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		reset();
	}

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
		m_current_speed -= 0.1f;
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
		m_current_speed += 0.1f;

	m_current_speed = fmax(0.f, m_current_speed);
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	// TODO: HANDLE MOUSE MOVE (IF NECESSARY)
}

void World::reset()
{
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	for (auto player : players) {
		player->destroy();
		player->init();
	}
	m_background.reset_player_dead_time();
	m_current_speed = 1.f;
}
