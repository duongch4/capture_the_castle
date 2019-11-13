// Header
#include "world.hpp"

// stlib
#include <cstring>
#include <cassert>
#include <sstream>
#include <states/game.hpp>
#include <states/menu.hpp>

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

}

World::~World()
{

}

// World initialization

bool World::init(vec2 screen)
{
	m_screen_size = screen;

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
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3)
	{
		((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3);
	};
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1)
	{
		((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1);
	};
	auto cursor_click_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2)
	{
		((World*)glfwGetWindowUserPointer(wnd))->on_mouse_click(wnd, _0, _1, _2);
	};
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(m_window, cursor_click_redirect);

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int fb_width, fb_height;
	glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
	m_screen_scale = static_cast<float>(fb_width) / screen.x;

    // Fake projection matrix, scales with respect to window coordinates
    // PS: 1.f / w in [1][1] is correct.. do you know why ? (:
    float left = 0.f;// *-0.5;
    float top = 0.f;// (float)h * -0.5;
    float right = (float)fb_width / m_screen_scale;// *0.5;
    float bottom = (float)fb_height / m_screen_scale;// *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    projection_2D = { { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };


	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	//--------------------------------------------------------------------------
	// Initializing state
	return set_state(new Menu());
}

bool World::set_state(State* new_state) {
    if (m_state != nullptr) {
        m_state->destroy();
    }
    m_state = new_state;
    return m_state->init_state(this);
}

// Releases all the associated resources
void World::destroy() {
    Mix_CloseAudio();
    m_state->destroy();
    TextureManager::instance()->unload_all_textures();
    MeshManager::instance().release_all();
    glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms) {
    return m_state->update(elapsed_ms);
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();
	m_state->draw();

    // Presenting
    glfwSwapBuffers(m_window);

}

// Should the game be over ?
bool World::is_over() const
{
	return glfwWindowShouldClose(m_window);
}

void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    m_state->on_key(key, action);
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    m_state->on_mouse_move(window, xpos, ypos);
}

void World::reset()
{
	// TODO: Handle world reset
	//    int w, h;
	//    glfwGetWindowSize(m_window, &w, &h);
	//    for (auto player : players) {
	//        player->destroy();
	//        player->init();
	//    }
	//    m_background.reset_player_dead_time();
	//    m_current_speed = 1.f;
}

void World::on_mouse_click(GLFWwindow *pWwindow, int button, int action, int mods) {
    m_state->on_mouse_click(pWwindow, button, action, mods);
}


vec2 World::get_screen_size() {
    return m_screen_size;
}

void World::get_buffer_size(int &w, int &h) {
    // Getting size of window
    glfwGetFramebufferSize(m_window, &w, &h);
}

mat3 World::get_projection_2d() {
    return projection_2D;
}

void World::set_window_closed() {
    glfwSetWindowShouldClose(m_window, 1);
}

void World::create_texture_from_window(Texture &tex) {
    tex.create_from_screen(m_window);
}

float World::get_screen_scale() {
    return m_screen_scale;
}

