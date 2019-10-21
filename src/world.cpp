// Header
#include "world.hpp"

// stlib
#include <cstring>
#include <cassert>
#include <sstream>



// Same as static in c, local to compilation unit
namespace {
    namespace {
        void glfw_err_cb(int error, const char *desc) {
            fprintf(stderr, "%d: %s", error, desc);
        }
    }
}

extern ECSManager ecsManager;

World::World(){

}

World::~World() {

}

// World initialization

bool World::init(vec2 screen)
{
	m_screen_size = screen;
	m_current_speed = 1.f;

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
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) {
		((World *)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3);
	};
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
		((World *)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1);
	};
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
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	//--------------------------------------------------------------------------
	// Initializing game

	// Tilemap initialization
	tilemap = std::make_shared<Tilemap>();
	tilemap->init();

	// ECS initialization
	// Register ALL Components
	ecsManager.registerComponent<Motion>();
	ecsManager.registerComponent<Transform>();
	ecsManager.registerComponent<Team>();
	ecsManager.registerComponent<Effect>();
	ecsManager.registerComponent<Sprite>();
	ecsManager.registerComponent<Mesh>();
	ecsManager.registerComponent<BanditSpawnComponent>();
	ecsManager.registerComponent<PlayerInputControlComponent>();
	ecsManager.registerComponent<PlaceableComponent>();

	ecsManager.registerComponent<BanditAIComponent>();

	// Register Systems and Entities
	movementSystem = ecsManager.registerSystem<MovementSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<Motion>());
		signature.set(ecsManager.getComponentType<Transform>());
		ecsManager.setSystemSignature<MovementSystem>(signature);
	}
	movementSystem->init();

	playerInputSystem = ecsManager.registerSystem<PlayerInputSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<Motion>());
		signature.set(ecsManager.getComponentType<Transform>());
		signature.set(ecsManager.getComponentType<Team>());
		signature.set(ecsManager.getComponentType<PlayerInputControlComponent>());
		ecsManager.setSystemSignature<PlayerInputSystem>(signature);
	}
	playerInputSystem->init();

	spriteRenderSystem = ecsManager.registerSystem<SpriteRenderSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<Effect>());
		signature.set(ecsManager.getComponentType<Sprite>());
		signature.set(ecsManager.getComponentType<Mesh>());
		signature.set(ecsManager.getComponentType<Transform>());
		ecsManager.setSystemSignature<SpriteRenderSystem>(signature);
	}
	spriteRenderSystem->init();

	banditSpawnSystem = ecsManager.registerSystem<BanditSpawnSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<BanditSpawnComponent>());
		ecsManager.setSystemSignature<BanditSpawnSystem>(signature);
	}
	banditSpawnSystem->init(tilemap);

	// Team 1 Soldiers
	Entity soldier1Team1 = ecsManager.createEntity();
	ecsManager.addComponent<Team>(soldier1Team1, Team{ TeamType::PLAYER1 });
	ecsManager.addComponent<PlaceableComponent>(soldier1Team1, PlaceableComponent{});
	ecsManager.addComponent<Transform>(soldier1Team1, Transform{
		tilemap->get_random_free_tile_position(MazeRegion::PLAYER1),
		{0.08f, 0.08f}
		});
	Effect soldier1Team1Effect{};
	soldier1Team1Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(soldier1Team1, soldier1Team1Effect);
	Sprite soldier1Team1Sprite = { textures_path("red_soldier_sprite_sheet-01.png") };
	TextureManager::instance()->load_from_file(soldier1Team1Sprite);
	soldier1Team1Sprite.sprite_index = { 0 , 0 };
	soldier1Team1Sprite.sprite_size = { soldier1Team1Sprite.width / 7.0f , soldier1Team1Sprite.height / 5.0f };
	ecsManager.addComponent<Sprite>(soldier1Team1, soldier1Team1Sprite);
	Mesh soldier1Team1Mesh{};
	soldier1Team1Mesh.init(soldier1Team1Sprite.width, soldier1Team1Sprite.height, soldier1Team1Sprite.sprite_size.x, soldier1Team1Sprite.sprite_size.y,
		soldier1Team1Sprite.sprite_index.x, soldier1Team1Sprite.sprite_index.y, 0);
	ecsManager.addComponent<Mesh>(soldier1Team1, soldier1Team1Mesh);

	// Team 2 Soldiers
	Entity soldier1Team2 = ecsManager.createEntity();
	ecsManager.addComponent<Team>(soldier1Team2, Team{ TeamType::PLAYER2 });
	ecsManager.addComponent<PlaceableComponent>(soldier1Team2, PlaceableComponent{});
	ecsManager.addComponent<Transform>(soldier1Team2, Transform{
			tilemap->get_random_free_tile_position(MazeRegion::PLAYER2),
			{0.08f, 0.08f}
		});
	Effect soldier1Team2Effect{};
	soldier1Team2Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(soldier1Team2, soldier1Team2Effect);
	Sprite soldier1Team2Sprite = { textures_path("blue_soldier_sprite_sheet-01.png") };
	TextureManager::instance()->load_from_file(soldier1Team2Sprite);
	soldier1Team2Sprite.sprite_index = { 0 , 0 };
	soldier1Team2Sprite.sprite_size = { soldier1Team2Sprite.width / 7.0f , soldier1Team2Sprite.height / 5.0f };
	ecsManager.addComponent<Sprite>(soldier1Team2, soldier1Team2Sprite);
	Mesh soldier1Team2Mesh{};
	soldier1Team2Mesh.init(soldier1Team2Sprite.width, soldier1Team2Sprite.height, soldier1Team2Sprite.sprite_size.x, soldier1Team2Sprite.sprite_size.y, 
		soldier1Team2Sprite.sprite_index.x, soldier1Team2Sprite.sprite_index.y, 0);
	ecsManager.addComponent<Mesh>(soldier1Team2, soldier1Team2Mesh);

	// CASTLE 1
	Entity castle1 = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(castle1, Transform{
			{ 120.f, m_screen_size.y / 2 },
			{0.5f, 0.5f}
		});
	ecsManager.addComponent<Team>(castle1, Team{ TeamType::PLAYER1 });
	Effect castle1Effect{};
	castle1Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(castle1, castle1Effect);
	Sprite castle1Sprite = { textures_path("castle/CaptureTheCastle_castle_red.png") };
	TextureManager::instance()->load_from_file(castle1Sprite);
	ecsManager.addComponent<Sprite>(castle1, castle1Sprite);
	Mesh castle1Mesh{};
	castle1Mesh.init(castle1Sprite.width, castle1Sprite.height);
	ecsManager.addComponent<Mesh>(castle1, castle1Mesh);

	// CASTLE 2
	Entity castle2 = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(castle2, Transform{
			{m_screen_size.x - 120.f, m_screen_size.y / 2},
			{0.5f, 0.5f}
		});
	ecsManager.addComponent<Team>(castle2, Team{ TeamType::PLAYER2 });
	Effect castle2Effect{};
	castle2Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(castle2, castle2Effect);
	Sprite castle2Sprite = { textures_path("castle/CaptureTheCastle_castle_blue.png") };
	TextureManager::instance()->load_from_file(castle2Sprite);
	ecsManager.addComponent<Sprite>(castle2, castle2Sprite);
	Mesh castle2Mesh{};
	castle2Mesh.init(castle2Sprite.width, castle2Sprite.height);
	ecsManager.addComponent<Mesh>(castle2, castle2Mesh);

	// PLAYER 1
	Entity player1 = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(player1, Transform{
		{ 120.f, m_screen_size.y / 2 + 130.f },
		{0.09f, 0.09f}
		});
	ecsManager.addComponent<Motion>(player1, Motion{
			{0, 0},
			100.f
		});
	ecsManager.addComponent<Team>(player1, Team{ TeamType::PLAYER1 });
	ecsManager.addComponent<PlayerInputControlComponent>(player1, PlayerInputControlComponent{});
	Effect player1Effect{};
	player1Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(player1, player1Effect);
	Sprite player1Sprite = { textures_path("red_king_sprite_sheet.png") };
	TextureManager::instance()->load_from_file(player1Sprite);
	player1Sprite.sprite_index = { 0 , 0 };
	player1Sprite.sprite_size = { player1Sprite.width / 7.0f , player1Sprite.height / 5.0f };
	ecsManager.addComponent<Sprite>(player1, player1Sprite);
	Mesh player1Mesh{};
	player1Mesh.init(player1Sprite.width, player1Sprite.height, player1Sprite.sprite_size.x, player1Sprite.sprite_size.y, player1Sprite.sprite_index.x, player1Sprite.sprite_index.y, 0);
	ecsManager.addComponent<Mesh>(player1, player1Mesh);

	// PLAYER 2
	Entity player2 = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(player2, Transform{
		{ m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
		{0.09f, 0.09f}
		});
	ecsManager.addComponent<Motion>(player2, Motion{
			{0, 0},
			100.f
		});
	ecsManager.addComponent<Team>(player2, Team{ TeamType::PLAYER2 });
	ecsManager.addComponent<PlayerInputControlComponent>(player2, PlayerInputControlComponent{});
	Effect player2Effect{};
	player2Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(player2, player2Effect);
	Sprite player2Sprite = { textures_path("blue_king_sprite_sheet.png") };
	TextureManager::instance()->load_from_file(player2Sprite);
	player2Sprite.sprite_size = { player2Sprite.width / 7.0f , player2Sprite.height / 5.0f };
	player2Sprite.sprite_index = { 0 , 0 };
	ecsManager.addComponent<Sprite>(player2, player2Sprite);
	Mesh player2Mesh{};
	player2Mesh.init(player2Sprite.width, player2Sprite.height, player2Sprite.sprite_size.x, player2Sprite.sprite_size.y, player2Sprite.sprite_index.x, player2Sprite.sprite_index.y, 0);
	ecsManager.addComponent<Mesh>(player2, player2Mesh);

  	// AI
	banditAISystem = ecsManager.registerSystem<BanditAISystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<BanditAIComponent>());
		ecsManager.setSystemSignature<BanditAISystem>(signature);
	}
	banditAISystem->init(tilemap, player1, player2);

    // ITEM BOARD (PLAYER 1)
    Entity player1_board = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(player1_board, Transform{
            { 180.f, 65 },
            {0.5f, 0.5f}
    });
    ecsManager.addComponent<Team>(player1_board, Team{TeamType::PLAYER1});
    Effect player1BoardEffect{};
    player1BoardEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(player1_board, player1BoardEffect);
    Sprite player1BoardSprite = {textures_path("ui/CaptureTheCastle_player_tile_red.png")};
    TextureManager::instance()->load_from_file(player1BoardSprite);
    ecsManager.addComponent<Sprite>(player1_board, player1BoardSprite);
    Mesh player1BoardMesh{};
    player1BoardMesh.init(player1BoardSprite.width, player1BoardSprite.height);
    ecsManager.addComponent<Mesh>(player1_board, player1BoardMesh);

    // ITEM BOARD (PLAYER 2)
    Entity player2_board = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(player2_board, Transform{
            { screen.x - 180.f, 65.f },
            {0.5f, 0.5f}
    });
    ecsManager.addComponent<Team>(player2_board, Team{TeamType::PLAYER2});
    Effect player2BoardEffect{};
    player2BoardEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(player2_board, player2BoardEffect);
    Sprite player2BoardSprite = {textures_path("ui/CaptureTheCastle_player_tile_blue.png")};
    TextureManager::instance()->load_from_file(player2BoardSprite);
    ecsManager.addComponent<Sprite>(player2_board, player2BoardSprite);
    Mesh player2BoardMesh{};
    player2BoardMesh.init(player2BoardSprite.width, player2BoardSprite.height);
    ecsManager.addComponent<Mesh>(player2_board, player2BoardMesh);

	//m_background.init();

	//--------------------------------------------------------------------------
	// Render all the tiles once to the screen texture

	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	// Bind the custom framebuffer with our screen texture
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 0.43f, 0.92f, 0.51f };
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

	// Render each tile to our screen texture
	tilemap->draw_all_tiles(projection_2D);

    return true;
}

// Releases all the associated resources
void World::destroy() {
    glDeleteFramebuffers(1, &m_frame_buffer);

    // TODO: MIX_FREEAUDIO AND MIX_FREECHUNK ON ALL AUDIOS
    Mix_CloseAudio();
    tilemap->destroy();
    glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms) {
    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
    vec2 screen = {(float) w / m_screen_scale, (float) h / m_screen_scale};

    banditSpawnSystem->update(elapsed_ms);
	banditAISystem->update(elapsed_ms);
    playerInputSystem->update();
    movementSystem->update(elapsed_ms);
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

	/////////////////////
	// Truely render to the screen
	// Unbind our custom frame buffer in init function and switch it back
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(0.43f, 0.92f, 0.51f, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_screen_tex.id); // screen texture

	// Background
	//m_background.draw(projection_2D);

	// Render the screen texture with all our tiles on it
	tilemap->draw(projection_2D);

	// Render the remaining entities on top our screen texture
    spriteRenderSystem->draw(projection_2D);

    // Presenting
    glfwSwapBuffers(m_window);

}

// Should the game be over ?
bool World::is_over() const {
    return glfwWindowShouldClose(m_window);
}

void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
    InputKeys k = InputKeys :: DEFAULT;
    switch (key)
    {
        case GLFW_KEY_DOWN:
            k = InputKeys::DOWN;
            break;
        case GLFW_KEY_UP:
            k = InputKeys::UP;
            break;
        case GLFW_KEY_LEFT:
            k = InputKeys::LEFT;
            break;
        case GLFW_KEY_RIGHT:
            k = InputKeys::RIGHT;
            break;
        case GLFW_KEY_S:
            k =InputKeys::S;
            break;
        case GLFW_KEY_W:
            k = InputKeys::W;
            break;
        case GLFW_KEY_D:
            k =InputKeys::D;
            break;
        case GLFW_KEY_A:
            k = InputKeys::A;
            break;
        default:
            break;
    }
    if (action == GLFW_PRESS && k != InputKeys::DEFAULT) {
        ecsManager.publish(new InputKeyEvent(k));
    } else if (action == GLFW_RELEASE && k != InputKeys::DEFAULT) {
        ecsManager.publish(new KeyReleaseEvent(k));
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

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    // TODO: HANDLE MOUSE MOVE (IF NECESSARY)
}

void World::reset() {
//    int w, h;
//    glfwGetWindowSize(m_window, &w, &h);
//    for (auto player : players) {
//        player->destroy();
//        player->init();
//    }
//    m_background.reset_player_dead_time();
//    m_current_speed = 1.f;
}