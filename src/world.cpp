// Header
#include "world.hpp"

// stlib
#include <cstring>
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

extern ECSManager ecsManager;

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
	currState = WorldState::NORMAL;

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	if (!initAssetsOpenGL(screen)) return false;

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (!loadAudio()) return false;

	//--------------------------------------------------------------------------
	// Initializing game

	// Tilemap initialization
	tilemap = std::make_shared<Tilemap>();
	tilemap->init();

	// ECS initialization
	// Register ALL Components
	registerComponents();

	// Register Systems and Entities
	registerMovementSystem(screen);
	registerPlayerInputSystem();
	registerSpriteRenderSystem();
	registerBanditSpawnSystem();
	registerCollisionSystem();
	registerBoxCollisionSystem();

	// Castles
	registerCastles();

	// Players
	std::vector<Entity> players;
	registerPlayers(players);

	// Bandit AI System
	if (players.size() < 2) return false;
	registerBanditAiSystem(players[0], players[1]);

	// Soldiers
	registerSoldiers();

	// Item boards
	registerItemBoards(screen);

	// HELP BUTTON
	help_btn.init(m_screen_size);

	// Help Window Initialization
	help_window.init(m_screen_size);

	// Winner's Window Initialization
	win_window.init(m_screen_size);

	ecsManager.subscribe(this, &World::winListener);

	//--------------------------------------------------------------------------
	// Render all the tiles once to the screen texture
	renderTilesToScreenTexture();

	return true;
}

void World::renderTilesToScreenTexture()
{
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
}

void World::registerItemBoards(vec2& screen)
{
	// ITEM BOARD (PLAYER 1)
	Transform transform_itemboard1 = Transform{
		{ 180.f, 65 },
		{ 180.f, 65 },
		{ 0.5f, 0.5f },
		{ 180.f, 65 }
	};
	registerItemBoard(transform_itemboard1, TeamType::PLAYER1, textures_path("ui/CaptureTheCastle_player_tile_red.png"));

	// ITEM BOARD (PLAYER 2)
	Transform transform_itemboard2 = Transform{
		{ screen.x - 180.f, 65.f },
		{ screen.x - 180.f, 65.f },
		{ 0.5f, 0.5f },
		{ screen.x - 180.f, 65.f }
	};
	registerItemBoard(transform_itemboard2, TeamType::PLAYER2, textures_path("ui/CaptureTheCastle_player_tile_blue.png"));
}

void World::registerSoldiers()
{
	Motion motion_soldier = Motion{ { 0, 0 }, 100.f };

	// Team 1 Soldiers
	vec2 pos1 = tilemap->get_random_free_tile_position(MazeRegion::PLAYER1);
	Transform transform_soldier1 = Transform{
		pos1,
		pos1,
		{ 0.08f, 0.08f },
		pos1
	};
	registerSoldier(transform_soldier1, motion_soldier, TeamType::PLAYER1, textures_path("red_soldier_sprite_sheet-01.png"));

	// Team 2 Soldiers
	vec2 pos2 = tilemap->get_random_free_tile_position(MazeRegion::PLAYER2);
	Transform transform_soldier2 = Transform{
		pos2,
		pos2,
		{ 0.08f, 0.08f },
		pos2
	};
	registerSoldier(transform_soldier2, motion_soldier, TeamType::PLAYER2, textures_path("blue_soldier_sprite_sheet-01.png"));
}

void World::registerPlayers(std::vector<Entity>& players)
{
	// PLAYERS
	Motion motion_player = Motion{ { 0, 0 }, 100.f };

	// PLAYER 1
	Transform transform_player1 = Transform{
		{ 120.f, m_screen_size.y / 2 + 130.f },
		// { 120.f - castleWidth, m_screen_size.y / 2 - castleHeight},  debugging purpose
		{ 120.f, m_screen_size.y / 2 + 130.f },
		{ 0.09f, 0.09f },
		{ 120.f, m_screen_size.y / 2 + 130.f }
	};
	Entity player1 = registerPlayer(transform_player1, motion_player, TeamType::PLAYER1, textures_path("red_king_sprite_sheet.png"));
	players.emplace_back(player1);

	// PLAYER 2
	Transform transform_player2 = Transform{
		{ m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
		// { 120.f, m_screen_size.y / 2}, debugging purpose
		{ m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
		{ 0.09f, 0.09f },
		{ 120.f, m_screen_size.y / 2 + 130.f }
	};
	Entity player2 = registerPlayer(transform_player2, motion_player, TeamType::PLAYER2, textures_path("blue_king_sprite_sheet.png"));
	players.emplace_back(player2);
}

void World::registerCastles()
{
	// CASTLE 1
	Transform transform_castle1 = Transform{
		{ 120.f, m_screen_size.y / 2 },
		{ 120.f, m_screen_size.y / 2 },
		{ 0.5f, 0.5f },
		{ 120.f, m_screen_size.y / 2 }
	};
	registerCastle(transform_castle1, TeamType::PLAYER1, textures_path("castle/CaptureTheCastle_castle_red.png"));

	// CASTLE 2
	Transform transform_castle2 = Transform{
		{ m_screen_size.x - 120.f, m_screen_size.y / 2 },
		{ m_screen_size.x - 120.f, m_screen_size.y / 2 },
		{ 0.5f, 0.5f },
		{ m_screen_size.x - 120.f, m_screen_size.y / 2 }
	};
	registerCastle(transform_castle2, TeamType::PLAYER2, textures_path("castle/CaptureTheCastle_castle_blue.png"));
}

bool World::loadAudio()
{
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

	m_background_music = Mix_LoadMUS(audio_path("capturethecastle_background.wav"));

	if (m_background_music == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
			audio_path("music.wav"));
		return false;
	}

	// Playing background music indefinitely
	Mix_PlayMusic(m_background_music, -1);
	return true;
}

bool World::initAssetsOpenGL(vec2& screen)
{
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
	return true;
}

void World::registerItemBoard(const Transform& transform, const TeamType& team_type, const char* texture_path)
{
	Entity itemBoard = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(itemBoard, transform);
	ecsManager.addComponent<Team>(itemBoard, Team{ team_type });
	Effect itemBoardEffect{};
	itemBoardEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(itemBoard, itemBoardEffect);
	Sprite itemBoardSprite = { texture_path };
	TextureManager::instance()->load_from_file(itemBoardSprite);
	ecsManager.addComponent<Sprite>(itemBoard, itemBoardSprite);
	Mesh itemBoardMesh{};
	itemBoardMesh.init(itemBoardSprite.width, itemBoardSprite.height);
	ecsManager.addComponent<Mesh>(itemBoard, itemBoardMesh);
}

void World::registerSoldier(const Transform& transform, const Motion& motion, const TeamType& team_type, const char* texture_path)
{
	Entity soldier = ecsManager.createEntity();
	ecsManager.addComponent<Team>(soldier, Team{ team_type });
	ecsManager.addComponent<PlaceableComponent>(soldier, PlaceableComponent{});
	ecsManager.addComponent<Transform>(soldier, transform);
	ecsManager.addComponent<Motion>(soldier, motion);
	Effect soldierEffect{};
	soldierEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(soldier, soldierEffect);
	Sprite soldierSprite = { texture_path };
	TextureManager::instance()->load_from_file(soldierSprite);
	soldierSprite.sprite_index = { 0 , 0 };
	soldierSprite.sprite_size = { soldierSprite.width / 7.0f , soldierSprite.height / 5.0f };
	ecsManager.addComponent<Sprite>(soldier, soldierSprite);
	Mesh soldierMesh{};
	soldierMesh.init(
		soldierSprite.width, soldierSprite.height, soldierSprite.sprite_size.x, soldierSprite.sprite_size.y,
		soldierSprite.sprite_index.x, soldierSprite.sprite_index.y, 0
	);
	ecsManager.addComponent<Mesh>(soldier, soldierMesh);
	ecsManager.addComponent(
		soldier,
		C_Collision{
			CollisionLayer::Enemy,
			soldierSprite.width / 2 * 0.08f,
			{ soldierSprite.width * 0.08f * 0.8f, soldierSprite.height * 0.08f * 0.8f }
		}
	);
}

Entity World::registerPlayer(const Transform& transform, const Motion& motion, const TeamType& team_type, const char* texture_path)
{
	Entity player = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(player, transform);
	ecsManager.addComponent<Motion>(player, motion);
	ecsManager.addComponent<Team>(player, Team{ team_type });
	ecsManager.addComponent<PlayerInputControlComponent>(player, PlayerInputControlComponent{});
	Effect playerEffect{};
	playerEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(player, playerEffect);
	Sprite playerSprite = { texture_path };
	TextureManager::instance()->load_from_file(playerSprite);
	playerSprite.sprite_index = { 0 , 0 };
	playerSprite.sprite_size = { playerSprite.width / 7.0f , playerSprite.height / 5.0f };
	ecsManager.addComponent<Sprite>(player, playerSprite);
	Mesh playerMesh{};
	playerMesh.init(playerSprite.width, playerSprite.height, playerSprite.sprite_size.x, playerSprite.sprite_size.y, playerSprite.sprite_index.x, playerSprite.sprite_index.y, 0);
	ecsManager.addComponent<Mesh>(player, playerMesh);
	ecsManager.addComponent(
		player,
		C_Collision{
			CollisionLayer::PLAYER1,
			playerSprite.width / 2 * 0.09f,
			{ playerSprite.width * 0.09f * 0.8f, playerSprite.height * 0.09f * 0.8f }
		}
	);
	return player;
}

void World::registerCastle(const Transform& transform, const TeamType& team_type, const char* texture_path)
{
	Entity castle = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(castle, transform);
	ecsManager.addComponent<Team>(castle, Team{ team_type });
	Effect castleEffect{};
	castleEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(castle, castleEffect);
	Sprite castleSprite = { texture_path };
	TextureManager::instance()->load_from_file(castleSprite);
	ecsManager.addComponent<Sprite>(castle, castleSprite);
	Mesh castleMesh{};
	castleMesh.init(castleSprite.width, castleSprite.height);
	ecsManager.addComponent<Mesh>(castle, castleMesh);
	ecsManager.addComponent(
		castle,
		C_Collision{
			CollisionLayer::Castle,
			0,
			{ castleSprite.width * 0.2f, castleSprite.height * 0.2f }
		}
	);
}

void World::registerBanditAiSystem(Entity& player1, Entity& player2)
{
	banditAISystem = ecsManager.registerSystem<BanditAISystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<BanditAIComponent>());
		ecsManager.setSystemSignature<BanditAISystem>(signature);
	}
	banditAISystem->init(tilemap, player1, player2);
}

void World::registerBoxCollisionSystem()
{
	boxCollisionSystem = ecsManager.registerSystem<BoxCollisionSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<Transform>());
		signature.set(ecsManager.getComponentType<C_Collision>());
		signature.set(ecsManager.getComponentType<Motion>());
		ecsManager.setSystemSignature<BoxCollisionSystem>(signature);
	}
	boxCollisionSystem->init(tilemap);
}

void World::registerCollisionSystem()
{
	collisionSystem = ecsManager.registerSystem<CollisionSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<Transform>());
		signature.set(ecsManager.getComponentType<Team>());
		signature.set(ecsManager.getComponentType<C_Collision>());
		// signature.set(ecsManager.getComponentType<Motion>());
		ecsManager.setSystemSignature<CollisionSystem>(signature);
	}
	collisionSystem->init();
}

void World::registerBanditSpawnSystem()
{
	banditSpawnSystem = ecsManager.registerSystem<BanditSpawnSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<BanditSpawnComponent>());
		ecsManager.setSystemSignature<BanditSpawnSystem>(signature);
	}
	banditSpawnSystem->init(tilemap);
}

void World::registerSpriteRenderSystem()
{
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
}

void World::registerPlayerInputSystem()
{
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
}

void World::registerMovementSystem(const vec2& screen)
{
	movementSystem = ecsManager.registerSystem<MovementSystem>();
	{
		Signature signature;
		signature.set(ecsManager.getComponentType<Motion>());
		signature.set(ecsManager.getComponentType<Transform>());
		ecsManager.setSystemSignature<MovementSystem>(signature);
	}
	movementSystem->init();
	movementSystem->setScreenSize(screen);

}

void World::registerComponents()
{
	ecsManager.registerComponent<Motion>();
	ecsManager.registerComponent<Transform>();
	ecsManager.registerComponent<Team>();
	ecsManager.registerComponent<Effect>();
	ecsManager.registerComponent<Sprite>();
	ecsManager.registerComponent<Mesh>();
	ecsManager.registerComponent<C_Collision>();
	ecsManager.registerComponent<BanditSpawnComponent>();
	ecsManager.registerComponent<PlayerInputControlComponent>();
	ecsManager.registerComponent<PlaceableComponent>();
	ecsManager.registerComponent<BanditAIComponent>();
}

// Releases all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	Mix_CloseAudio();
	tilemap->destroy();
	help_btn.destroy();
	help_window.destroy();
	TextureManager::instance()->unload_all_textures();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w / m_screen_scale, (float)h / m_screen_scale };

	if (currState == WorldState::NORMAL)
	{
		banditSpawnSystem->update(elapsed_ms);
		banditAISystem->update(elapsed_ms);
		playerInputSystem->update();
		collisionSystem->checkCollision();
		collisionSystem->update();
		boxCollisionSystem->checkCollision();
		boxCollisionSystem->update();
		movementSystem->update(elapsed_ms);
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
	// Truly render to the screen
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
	help_btn.draw(projection_2D);

	if (currState == WorldState::HELP)
	{
		help_window.draw(projection_2D);
	}

	if (currState == WorldState::WIN)
	{
		win_window.draw(projection_2D);
	}

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
	InputKeys k = InputKeys::DEFAULT;
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
		k = InputKeys::S;
		break;
	case GLFW_KEY_W:
		k = InputKeys::W;
		break;
	case GLFW_KEY_D:
		k = InputKeys::D;
		break;
	case GLFW_KEY_A:
		k = InputKeys::A;
		break;
	default:
		break;
	}
	if (action == GLFW_PRESS && k != InputKeys::DEFAULT)
	{
		ecsManager.publish(new InputKeyEvent(k));
	}
	else if (action == GLFW_RELEASE && k != InputKeys::DEFAULT)
	{
		ecsManager.publish(new KeyReleaseEvent(k));
	}

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		reset();
	}
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	if (currState == WorldState::NORMAL)
	{
		help_btn.onHover(help_btn.mouseOnButton({ (float)xpos, (float)ypos }));
	}
	if (currState == WorldState::HELP)
	{
		help_window.checkButtonHovers({ (float)xpos, (float)ypos });
	}
	if (currState == WorldState::WIN)
	{
		win_window.checkButtonHovers({ (float)xpos, (float)ypos });
	}
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

void World::on_mouse_click(GLFWwindow* pWwindow, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(pWwindow, &xpos, &ypos);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (currState == WorldState::NORMAL && help_btn.mouseOnButton({ (float)xpos, (float)ypos }))
		{
			currState = WorldState::HELP;
		}
		else if (currState == WorldState::HELP)
		{
			switch (help_window.checkButtonClicks({ (float)xpos, (float)ypos }))
			{
			case (ButtonActions::CLOSE):
				currState = WorldState::NORMAL;
				break;
			case (ButtonActions::HOWTOPLAY):
				break;
			default:
				break;
			}
		}
		else if (currState == WorldState::WIN)
		{
			switch (win_window.checkButtonClicks({ (float)xpos, (float)ypos }))
			{
			case (ButtonActions::MAIN):
				break;
			case (ButtonActions::QUIT):
				glfwSetWindowShouldClose(m_window, 1);
				break;
			case (ButtonActions::RESTART):
				reset();
				break;
			default:
				break;
			}
		}
	}
}

void World::winListener(WinEvent* winEvent)
{
	Team winningTeam = ecsManager.getComponent<Team>(winEvent->player);
	win_window.setWinTeam(winningTeam.assigned);
	currState = WorldState::WIN;
}

