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
//	m_salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav"));
//	m_salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav"));
//
//	if (m_background_music == nullptr || m_salmon_dead_sound == nullptr || m_salmon_eat_sound == nullptr)
//	{
//		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n make sure the data directory is present",
//			audio_path("music.wav"),
//			audio_path("salmon_dead.wav"),
//			audio_path("salmon_eat.wav"));
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
			if (!spawn_tile(data[iter], 6, 4, i, j))
				return false;

			Tile& new_tile = m_tiles[j][i];

			// Setting the tile initial position
			new_tile.set_position({ i * 46.f + 23.f, j * 43.f + 19.f });
		}
	}

	// TODO: CALL INIT ON ALL GAME ENTITIES
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

	/////////////////////
	// Truely render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);

	// Render all the tiles we have 
	for (auto& vector : m_tiles) {
		for (auto& tile : vector) {
			tile.draw(projection_2D);
		}
	}
		
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
bool World::spawn_tile(int id, int width, int height, int gridX, int gridY)
{
	Tile tile;
	if (tile.init(id, width, height))
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
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	// TODO: HANDLE MOUSE MOVE (IF NECESSARY)
}
