
#include "game.hpp"

Game::Game() = default;

bool Game::init_state(World* world) {
    m_world = world;
    tilemap = std::make_shared<Tilemap>();

    // Create a frame buffer
    m_frame_buffer = 0;
    glGenFramebuffers(1, &m_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

    // Initialize the screen texture
    m_world->create_texture_from_window(m_screen_tex);

    // ECS initialization
    // Register ALL Components
    registerComponents();

    // Register Systems and Entities
    registerMovementSystem(world->get_screen_size());
    registerPlayerInputSystem();
    registerSpriteRenderSystem();
    registerBanditSpawnSystem();
    registerCollisionSystem();
    registerBoxCollisionSystem();
    registerBanditAiSystem();
    registerSoldierAiSystem();
    registerItemSpawnSystem();
    registerItemBoardSystem();
    registerCurveMovementSystem();
    registerItemEffectSystem();

    ecsManager.subscribe(this, &Game::winListener);
	ecsManager.subscribe(this, &Game::flagListener);
    return init_game();
}

bool Game::init_game() {
    // Initializing game
    currState = GameState::START;
    m_screen_size = m_world->get_screen_size();

    //--------------------------------------------------------------------------
    // Initializing game
    // Tilemap initialization
    tilemap->init();

    // Castles
    registerCastles();

    // Players
    std::vector<Entity> players;
    registerPlayers(players);
    if (players.size() < 2) return false;

    banditAiSystem->init(tilemap, players);
    soldierAiSystem->init(tilemap, players);

    // Item boards
    registerItemBoards(m_world->get_screen_size());

    // Help Button Initialization
    help_btn.init(m_screen_size);

    // Help Window Initialization
    help_window.init(m_screen_size);

    // Winner's Window Initialization
    win_window.init(m_screen_size);

    // Pause Window Initialization
    pause_window.init(m_screen_size);

    // SetUp Window Initialization
    setup_window.init(m_screen_size);

    // Firework particle
    firework.init(m_screen_size);

    // Initialize Setup Instructions
    p1SetUpInstructions.init({120, 200}, textures_path("ui/CaptureTheCastle_soldier_setting_instructions_p1.png"));
    p2SetUpInstructions.init({m_screen_size.x - 120, 200}, textures_path("ui/CaptureTheCastle_soldier_setting_instructions_p2.png"));
    p1SetUpInstructions.setScale({0.2, 0.2});
    p2SetUpInstructions.setScale({0.2, 0.2});
    timer.init(m_screen_size);

    //--------------------------------------------------------------------------
    // Render all the tiles once to the screen texture
    renderTilesToScreenTexture();

    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
    m_background_music = Mix_LoadMUS(audio_path("capturethecastle_background.wav"));
	flag_sound = Mix_LoadWAV(audio_path("capturethecastle_flag_taken.wav"));

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

bool Game::update(float elapsed_ms) {
    if (currState == GameState::NORMAL || currState == GameState::FLAG) {
        banditSpawnSystem->update(elapsed_ms);
        banditAiSystem->update(elapsed_ms);
        playerInputSystem->update();
        collisionSystem->checkCollision();
        collisionSystem->update();
        boxCollisionSystem->checkCollision();
        boxCollisionSystem->update();
        movementSystem->update(elapsed_ms);
        itemSpawnSystem->update(elapsed_ms);
        soldierAiSystem->update(elapsed_ms);
        itemBoardSystem->update();
        curveMovementSystem->update(elapsed_ms);
        itemEffectSystem->update();
    } else if (currState == GameState::SETUP) {
        playerInputSystem->update();
        collisionSystem->checkCollision();
        collisionSystem->update();
        boxCollisionSystem->checkCollision();
        boxCollisionSystem->update();
        movementSystem->update(elapsed_ms);
        timer.update(elapsed_ms);
        if (timer.check_times_up()) {
            ecsManager.publish(new TimeoutEvent());
            currState = GameState::NORMAL;
        }
    } else if (currState == GameState::WIN) {
        firework.update(elapsed_ms);
    }
    return true;
}

void Game::draw() {
    // Getting size of window
    int w, h;
    m_world->get_buffer_size(w, h);

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

    mat3 projection_2D = m_world->get_projection_2d();
    // Render the screen texture with all our tiles on it
    tilemap->draw(projection_2D);

    // Render the remaining entities on top our screen texture
    spriteRenderSystem->draw(projection_2D);

    if (currState == GameState::HELP) {
        help_window.draw(projection_2D);
        help_btn.draw(projection_2D);
    } else if (currState == GameState::WIN) {
        win_window.draw(projection_2D);
        firework.draw(projection_2D);
    } else if (currState == GameState::PAUSE) {
        pause_window.draw(projection_2D);
        help_btn.draw(projection_2D);
    } else if (currState == GameState::START) {
        setup_window.draw(projection_2D);
        p1SetUpInstructions.draw(projection_2D);
        p2SetUpInstructions.draw(projection_2D);
    } else if (currState == GameState::SETUP) {
        timer.draw(projection_2D);
        p1SetUpInstructions.draw(projection_2D);
        p2SetUpInstructions.draw(projection_2D);
    } else if ((currState == GameState::NORMAL || currState == GameState::FLAG)) {
        help_btn.draw(projection_2D);
    }
}

void Game::on_key(int key, int action) {
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
		case GLFW_KEY_Q:
			k = InputKeys::Q;
			break;
		case GLFW_KEY_SLASH:
			k = InputKeys::SLASH;
			break;
        case GLFW_KEY_LEFT_SHIFT:
            k = InputKeys::LEFT_SHIFT;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            k = InputKeys::RIGHT_SHIFT;
            break;
        case GLFW_KEY_ESCAPE:
            k = InputKeys::ESC;
            break;
        default:
            break;
    }
    if (action == GLFW_PRESS && !(k == InputKeys::ESC || k == InputKeys::DEFAULT)) {
        if ((currState == GameState::NORMAL || currState == GameState::FLAG) && !(k == InputKeys::SLASH || k == InputKeys::Q)) {
            ecsManager.publish(new InputKeyEvent(k));
        } else if (currState == GameState::SETUP) {
            ecsManager.publish(new InputKeyEvent(k));
        }
    } else if (action == GLFW_RELEASE && !(k == InputKeys::DEFAULT || k == InputKeys::ESC)) {
        if ((currState == GameState::NORMAL || currState == GameState::FLAG) && !(k == InputKeys::SLASH || k == InputKeys::Q)) {
            ecsManager.publish(new KeyReleaseEvent(k));
        } else if (currState == GameState::SETUP) {
            ecsManager.publish(new KeyReleaseEvent(k));
        }
    } else if (action == GLFW_PRESS && k == InputKeys::ESC) {
        if (currState == GameState::NORMAL || currState == GameState::FLAG) {
			oldState = currState;
            currState = GameState::PAUSE;
        } else if(currState == GameState::PAUSE) {
            currState = oldState;
        }
    }
}

void Game::on_mouse_click(GLFWwindow *pWindow, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if ((currState == GameState::NORMAL || currState == GameState::FLAG) && help_btn.mouseOnButton({(float) xpos, (float) ypos })) {
            Mix_PlayChannel(-1, m_click, 0);
            currState = GameState :: HELP;
        } else if (currState == GameState::HELP) {
            switch (help_window.checkButtonClicks({ (float) xpos, (float) ypos }))
            {
                case (ButtonActions::CLOSE):
                    currState = GameState :: NORMAL;
                    help_window.resetWindow();
                    break;
                default:
                    break;
            }
        } else if (currState == GameState::WIN) {
            switch (win_window.checkButtonClicks({ (float) xpos, (float) ypos }))
            {
                case (ButtonActions::MAIN):
                    m_world->set_state(std::make_unique<Menu>());
                    break;
                case (ButtonActions::QUIT):
                    m_world->set_window_closed();
                    break;
                case (ButtonActions::RESTART):
                    reset();
                    break;
                default:
                    break;
            }
        } else if (currState == GameState::PAUSE) {
            switch(pause_window.checkButtonClicks({ (float) xpos, (float) ypos }))
            {
                case (ButtonActions::MAIN):
                    m_world->set_state(std::make_unique<Menu>());
                    break;
                case (ButtonActions::QUIT):
                    m_world->set_window_closed();
                    break;
                case (ButtonActions::RESTART):
                    reset();
                    break;
                case (ButtonActions::CLOSE):
                    currState = GameState::NORMAL;
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::S));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::W));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::A));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::D));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::UP));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::DOWN));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::LEFT));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::RIGHT));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::LEFT_SHIFT));
                    ecsManager.publish(new KeyReleaseEvent(InputKeys::RIGHT_SHIFT));
                    break;
                default:
                    break;
            }
        } else if(currState == GameState::START) {
            switch(setup_window.checkButtonClicks({ (float) xpos, (float) ypos }))
            {
                case (ButtonActions::START):
                    currState = SETUP;
                    timer.start_timer(30);
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    if (currState == GameState::NORMAL || currState == GameState::FLAG) {
        help_btn.onHover(help_btn.mouseOnButton({ (float)xpos, (float) ypos }));
    }
    else if (currState == GameState::HELP) {
        help_window.checkButtonHovers({ (float) xpos, (float) ypos });
    }
    else if (currState == GameState::WIN) {
        win_window.checkButtonHovers({ (float) xpos, (float) ypos });
    }
    else if (currState == GameState::PAUSE) {
        pause_window.checkButtonHovers({(float) xpos, (float) ypos});
    }
    else if (currState == GameState::START) {
        setup_window.checkButtonHovers({(float) xpos, (float) ypos});
    }
}

void Game::reset() {
    ecsManager.reset();
    std::cout << "ECS reset" << std::endl;
    tilemap->destroy();
	tilemap.reset();
    std::cout << "TileMap reset" << std::endl;
    help_btn.destroy();
    std::cout << "Help button destroyed" << std::endl;
    help_window.destroy();
    std::cout << "Help window destroyed" << std::endl;
    win_window.destroy();
    std::cout << "Win window destroyed" << std::endl;
    setup_window.destroy();
    std::cout << "Setup window destroyed" << std::endl;
    firework.destroy();
    std::cout << "Firework destroyed" << std::endl;
    pause_window.destroy();
    std::cout << "Pause window destroyed" << std::endl;
    p1SetUpInstructions.destroy();
    p2SetUpInstructions.destroy();
    std::cout << "SetUp Instructions destroyed" << std::endl;
    timer.destroy();
    if (m_background_music != nullptr)
        Mix_FreeMusic(m_background_music);
    if (m_click != nullptr)
        Mix_FreeChunk(m_click);
	if (flag_sound != nullptr)
	{
		Mix_FreeChunk(flag_sound);
	}
    std::cout << "Releasing music" << std::endl;
    std::cout << "Reinitializing game state" << std::endl;
    init_state(m_world);
}

void Game::winListener(WinEvent *winEvent) {
    // you should only win when in normal game mode, set up should not allow you to win
    if (currState == GameState::FLAG) {
        Team winningTeam = ecsManager.getComponent<Team>(winEvent->player);
        win_window.setWinTeam(winningTeam.assigned);
        currState = GameState :: WIN;
    }
}

void Game::flagListener(FlagEvent *flagEvent)
{
	if (flagEvent->flag && currState == GameState::NORMAL)
	{
		currState = GameState::FLAG;
		auto &team = ecsManager.getComponent<Team>(flagEvent->flagPlayer);
		const char* path;
		if (team.assigned == TeamType::PLAYER1)
		{
			path = flag_path("CaptureTheCastle_bubble_flag_blue.png");
		}
		else if (team.assigned == TeamType::PLAYER2)
		{
			path = flag_path("CaptureTheCastle_bubble_flag_red.png");
		}
		Entity bubble = registerBubble(flagEvent->flagPlayer, path);
		collisionSystem->setFlagMode(flagEvent->flagPlayer);
		playerInputSystem->setFlagMode(true, flagEvent->flagPlayer, bubble);
		boxCollisionSystem->setFlagMode(true, flagEvent->flagPlayer, bubble);
		Mix_PlayChannel(-1, flag_sound, 0);
	}
	else if(flagEvent->flag == false && currState == FLAG)
	{
		currState = GameState::NORMAL;
		playerInputSystem->setFlagMode(false, 0, 0);
		boxCollisionSystem->setFlagMode(false, 0, 0);
	}
}

void Game::destroy() {
    glDeleteFramebuffers(1, &m_frame_buffer);
    if (m_background_music != nullptr)
        Mix_FreeMusic(m_background_music);
    if (m_click != nullptr)
        Mix_FreeChunk(m_click);
    ecsManager.reset();
    tilemap->destroy();
	tilemap.reset();
    help_btn.destroy();
    help_window.destroy();
    win_window.destroy();
    pause_window.destroy();
    setup_window.destroy();
    p1SetUpInstructions.destroy();
    p2SetUpInstructions.destroy();
    timer.destroy();
    firework.destroy();
	itemSpawnSystem.reset();
	movementSystem.reset();
	playerInputSystem.reset();
	spriteRenderSystem.reset();
	banditSpawnSystem.reset();
	banditAiSystem.reset();
	soldierAiSystem.reset();
	collisionSystem.reset();
	boxCollisionSystem.reset();
	curveMovementSystem.reset();
	itemBoardSystem.reset();
	itemEffectSystem.reset();
}

Game::~Game() {
}

void Game::registerItemBoard(const Transform& transform, const TeamType& team_type, const char* texture_path)
{
    Entity itemBoard = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(itemBoard, transform);
    ecsManager.addComponent<Team>(itemBoard, Team{ team_type });
    Effect itemBoardEffect{};
    itemBoardEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(itemBoard, itemBoardEffect);
    Sprite itemBoardSprite = { texture_path };
    TextureManager::instance().load_from_file(itemBoardSprite);
    ecsManager.addComponent<Sprite>(itemBoard, itemBoardSprite);
    MeshComponent itemBoardMesh{};
    itemBoardMesh.id = MeshManager::instance().init_mesh(itemBoardSprite.width, itemBoardSprite.height);
    ecsManager.addComponent<MeshComponent>(itemBoard, itemBoardMesh);

    // Create Item on the Board
    Entity picked_up_item = ecsManager.createEntity();

    vec2 position = transform.position;
    if (team_type == TeamType::PLAYER1){
        position.x += 112;
    } else {
        position.x -= 115;
    }
    Transform item_transform = Transform{
            position,
            position,
            { 1.f, 1.f },
            position};
    ecsManager.addComponent<Transform>(picked_up_item, item_transform);
    ecsManager.addComponent<ItemBoardComponent>(picked_up_item, ItemBoardComponent{});
    ecsManager.addComponent<Team>(picked_up_item, Team{ team_type });
    Effect itemEffect{};
    itemEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(picked_up_item, itemEffect);
    Sprite itemSprite = {power_up_path("CaptureTheCastle_no_item.png")};
    TextureManager::instance().load_from_file(itemSprite);
    itemSprite.sprite_index = { 0 , 0 };
    itemSprite.sprite_size = { itemSprite.width / 7.0f , itemSprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(picked_up_item, itemSprite);

    MeshComponent itemMesh{MeshManager::instance().init_mesh(itemSprite.width, itemSprite.height)};

    ecsManager.addComponent<MeshComponent>(picked_up_item, itemMesh);
}

Entity Game::registerPlayer(const Transform& transform, const Motion& motion, const TeamType& team_type, const char* texture_path)
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
    TextureManager::instance().load_from_file(playerSprite);
    playerSprite.sprite_index = { 0 , 0 };
    playerSprite.sprite_size = { playerSprite.width / 7.0f , playerSprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(player, playerSprite);
    MeshComponent playerMesh{};
    playerMesh.id = MeshManager::instance().init_mesh(playerSprite.width, playerSprite.height, playerSprite.sprite_size.x, playerSprite.sprite_size.y, playerSprite.sprite_index.x, playerSprite.sprite_index.y, 0);
    ecsManager.addComponent<MeshComponent>(player, playerMesh);
    CollisionLayer collisionLayer;
    if (team_type == TeamType::PLAYER1){
        collisionLayer = CollisionLayer::PLAYER1;
    } else if (team_type == TeamType::PLAYER2){
        collisionLayer = CollisionLayer::PLAYER2;
    }
    ecsManager.addComponent(
            player,
            C_Collision{
                    collisionLayer,
                    playerSprite.width / 2 * 0.09f,
                    { playerSprite.width * 0.09f * 0.8f, playerSprite.height * 0.09f * 0.8f }
            }
    );
    ecsManager.addComponent<ItemComponent>(player,  ItemComponent{
            false,
            ItemType::None});
    return player;
}

Entity Game::registerBubble(Entity player, const char* texture_path)
{
	auto& transform = ecsManager.getComponent<Transform>(player);
	Transform transform_bubble = Transform{
		 { transform.position.x, transform.position.y - 20.f },
		 // { 120.f - castleWidth, m_screen_size.y / 2 - castleHeight},  debugging purpose
		 { transform.position.x, transform.position.y - 20.f },
		 {0.8f, 0.8f },
		 { transform.position.x, transform.position.y - 20.f }
	};
	auto& motion = ecsManager.getComponent<Motion>(player);
	auto& team = ecsManager.getComponent<Team>(player);
	Entity bubble = ecsManager.createEntity();
	ecsManager.addComponent<Transform>(bubble, transform_bubble);
	ecsManager.addComponent<Motion>(bubble, { {0.f, 0.f}, motion.speed });
	ecsManager.addComponent<Team>(bubble, team);

	Effect bubbleEffect{};
	bubbleEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(bubble, bubbleEffect);
	Sprite bubbleSprite = { texture_path };
	TextureManager::instance().load_from_file(bubbleSprite);
	bubbleSprite.sprite_size = { (float) bubbleSprite.width , (float)bubbleSprite.height};
	ecsManager.addComponent<Sprite>(bubble, bubbleSprite);
	MeshComponent bubbleMesh{};
	bubbleMesh.id = MeshManager::instance().init_mesh(bubbleSprite.width, bubbleSprite.height);
	ecsManager.addComponent<MeshComponent>(bubble, bubbleMesh);
	collisionSystem->setBubble(bubble);
	return bubble;
}

void Game::registerCastle(const Transform& transform, const TeamType& team_type, const char* texture_path)
{
    Entity castle = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(castle, transform);
    ecsManager.addComponent<Team>(castle, Team{ team_type });
    Effect castleEffect{};
    castleEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(castle, castleEffect);
    Sprite castleSprite = { texture_path };
    TextureManager::instance().load_from_file(castleSprite);
    ecsManager.addComponent<Sprite>(castle, castleSprite);
    MeshComponent castleMesh{};
    castleMesh.id = MeshManager::instance().init_mesh(castleSprite.width, castleSprite.height);
    ecsManager.addComponent<MeshComponent>(castle, castleMesh);
    ecsManager.addComponent(
            castle,
            C_Collision{
                    CollisionLayer::Castle,
                    0,
                    { castleSprite.width * 0.2f, castleSprite.height * 0.2f }
            }
    );
	//std::cout << castle << std::endl;
}

void Game::registerBanditAiSystem()
{
    banditAiSystem = ecsManager.registerSystem<BanditAiSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<BanditAiComponent>());
        ecsManager.setSystemSignature<BanditAiSystem>(signature);
    }
}

void Game::registerItemSpawnSystem()
{
    itemSpawnSystem = ecsManager.registerSystem<ItemSpawnSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<ItemComponent>());
        ecsManager.setSystemSignature<ItemSpawnSystem>(signature);
    }
    itemSpawnSystem->init(tilemap);
}

void Game::registerBoxCollisionSystem()
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

void Game::registerCollisionSystem()
{
    collisionSystem = ecsManager.registerSystem<CollisionSystem>();

        Signature signature;
        signature.set(ecsManager.getComponentType<Transform>());
        signature.set(ecsManager.getComponentType<Team>());
        signature.set(ecsManager.getComponentType<C_Collision>());
        // signature.set(ecsManager.getComponentType<Motion>());
        ecsManager.setSystemSignature<CollisionSystem>(signature);

    collisionSystem->init();
}

void Game::registerBanditSpawnSystem()
{
    banditSpawnSystem = ecsManager.registerSystem<BanditSpawnSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<BanditSpawnComponent>());
        ecsManager.setSystemSignature<BanditSpawnSystem>(signature);
    }
    banditSpawnSystem->init(tilemap);
}

void Game::registerSpriteRenderSystem()
{
    spriteRenderSystem = ecsManager.registerSystem<SpriteRenderSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<Effect>());
        signature.set(ecsManager.getComponentType<Sprite>());
        signature.set(ecsManager.getComponentType<MeshComponent>());
        signature.set(ecsManager.getComponentType<Transform>());
        ecsManager.setSystemSignature<SpriteRenderSystem>(signature);
    }
    spriteRenderSystem->init();
}

void Game::registerPlayerInputSystem()
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
    playerInputSystem->init(tilemap);
}

void Game::registerMovementSystem(const vec2& screen)
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

void Game::registerSoldierAiSystem() {
    soldierAiSystem = ecsManager.registerSystem<SoldierAiSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<SoldierAiComponent>());
        ecsManager.setSystemSignature<SoldierAiSystem>(signature);
    }
}

void Game::registerCurveMovementSystem()
{
    curveMovementSystem = ecsManager.registerSystem<CurveMovementSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<CurveMotionComponent>());
        signature.set(ecsManager.getComponentType<Transform>());
        ecsManager.setSystemSignature<CurveMovementSystem>(signature);
    }
    curveMovementSystem->init();
}
void Game::registerItemBoardSystem() {
    itemBoardSystem = ecsManager.registerSystem<ItemBoardSystem>();
    {
        Signature  signature;
        signature.set(ecsManager.getComponentType<ItemBoardComponent>());
        ecsManager.setSystemSignature<ItemBoardSystem>(signature);
    }
    itemBoardSystem->init();
}

void Game::registerItemEffectSystem() {
    itemEffectSystem = ecsManager.registerSystem<ItemEffectSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<ItemComponent>());
        signature.set(ecsManager.getComponentType<Transform>());
        ecsManager.setSystemSignature<ItemEffectSystem>(signature);
    }
    itemEffectSystem->init();
}

void Game::registerComponents()
{
    ecsManager.registerComponent<Motion>();
    ecsManager.registerComponent<Transform>();
    ecsManager.registerComponent<Team>();
    ecsManager.registerComponent<Effect>();
    ecsManager.registerComponent<Sprite>();
    ecsManager.registerComponent<MeshComponent>();
    ecsManager.registerComponent<C_Collision>();
    ecsManager.registerComponent<BanditSpawnComponent>();
    ecsManager.registerComponent<BanditAiComponent>();
    ecsManager.registerComponent<PlayerInputControlComponent>();
    ecsManager.registerComponent<SoldierAiComponent>();
    ecsManager.registerComponent<ItemComponent>();
    ecsManager.registerComponent<CurveMotionComponent>();
    ecsManager.registerComponent<ItemBoardComponent>();
    ecsManager.registerComponent<PlaceableComponent>();

}

void Game::registerCastles()
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

void Game::registerPlayers(std::vector<Entity>& players)
{
    // PLAYERS
    Motion motion_player = Motion{ { 0, 0 }, 100.f };

    // PLAYER 1
    Transform transform_player1 = Transform{
            { 120.f, m_screen_size.y / 2 + 130.f },
            // { 120.f - castleWidth, m_screen_size.y / 2 - castleHeight},  debugging purpose
            { 120.f, m_screen_size.y / 2 + 130.f },
            { 0.09f * 5 / 7, 0.09f },
            { 120.f, m_screen_size.y / 2 + 130.f }
    };
    Entity player1 = registerPlayer(transform_player1, motion_player, TeamType::PLAYER1, textures_path("red_king_sprite_sheet.png"));
    players.emplace_back(player1);

    // PLAYER 2
    Transform transform_player2 = Transform{
            { m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
            // { 120.f, m_screen_size.y / 2}, debugging purpose
            { m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
            { 0.09f * 5 / 7, 0.09f },
            { 120.f, m_screen_size.y / 2 + 130.f }
    };
    Entity player2 = registerPlayer(transform_player2, motion_player, TeamType::PLAYER2, textures_path("blue_king_sprite_sheet.png"));
    players.emplace_back(player2);
}

void Game::registerItemBoards(const vec2& screen)
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

void Game::renderTilesToScreenTexture()
{
    // Clearing error buffer
    gl_flush_errors();

    // Getting size of window
    int w, h;
    m_world->get_buffer_size(w, h);

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
    float right = (float)w / m_world->get_screen_scale();// *0.5;
    float bottom = (float)h / m_world->get_screen_scale();// *0.5;

    float sx = 2.f / (right - left);
    float sy = 2.f / (top - bottom);
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

    // Render each tile to our screen texture
    tilemap->draw_all_tiles(projection_2D);
}

