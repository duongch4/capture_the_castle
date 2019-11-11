//
// Created by Owner on 2019-11-06.
//

#include <mesh_manager.hpp>
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

    m_background_music = Mix_LoadMUS(audio_path("capturethecastle_background.wav"));

    if (m_background_music == nullptr)
    {
        fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
                audio_path("music.wav"));
        return false;
    }

    // Playing background music indefinitely
    Mix_PlayMusic(m_background_music, -1);

    // ECS initialization
    // Register ALL Components
    ecsManager.registerComponent<Motion>();
    ecsManager.registerComponent<Transform>();
    ecsManager.registerComponent<Team>();
    ecsManager.registerComponent<Effect>();
    ecsManager.registerComponent<Sprite>();
    ecsManager.registerComponent<MeshComponent>();
    ecsManager.registerComponent<C_Collision>();
    ecsManager.registerComponent<BanditSpawnComponent>();
    ecsManager.registerComponent<PlayerInputControlComponent>();
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
        signature.set(ecsManager.getComponentType<MeshComponent>());
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

    collisionSystem = ecsManager.registerSystem<CollisionSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<Transform>());
        signature.set(ecsManager.getComponentType<Team>());
        signature.set(ecsManager.getComponentType<C_Collision>());
//        signature.set(ecsManager.getComponentType<Motion>());
        ecsManager.setSystemSignature<CollisionSystem>(signature);
    }
    collisionSystem->init();

    boxCollisionSystem = ecsManager.registerSystem<BoxCollisionSystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<Transform>());
        signature.set(ecsManager.getComponentType<C_Collision>());
        signature.set(ecsManager.getComponentType<Motion>());
        ecsManager.setSystemSignature<BoxCollisionSystem>(signature);
    }
    boxCollisionSystem->init(tilemap);

    // AI
    banditAISystem = ecsManager.registerSystem<BanditAISystem>();
    {
        Signature signature;
        signature.set(ecsManager.getComponentType<BanditAIComponent>());
        ecsManager.setSystemSignature<BanditAISystem>(signature);
    }

    ecsManager.subscribe(this, &Game::winListener);

    return init_game();
}

bool Game::init_game() {
    // Initializing game
    currState = GameState::NORMAL;
    m_screen_size = m_world->get_screen_size();

    // Tilemap initialization
    tilemap->init();

    // Team 1 Soldiers
    Entity soldier1Team1 = ecsManager.createEntity();
    ecsManager.addComponent<Team>(soldier1Team1, Team{ TeamType::PLAYER1 });
    vec2 pos = tilemap->get_random_free_tile_position(MazeRegion::PLAYER1);
    ecsManager.addComponent<Transform>(soldier1Team1, Transform{
            pos,
            pos,
            {0.09f * 5 / 7, 0.09f},
            pos
    });
    Effect soldier1Team1Effect{};
    soldier1Team1Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(soldier1Team1, soldier1Team1Effect);
    Sprite soldier1Team1Sprite = { textures_path("red_soldier_sprite_sheet-01.png") };
    TextureManager::instance()->load_from_file(soldier1Team1Sprite);
    soldier1Team1Sprite.sprite_index = { 0 , 0 };
    soldier1Team1Sprite.sprite_size = { soldier1Team1Sprite.width / 7.0f , soldier1Team1Sprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(soldier1Team1, soldier1Team1Sprite);
    MeshComponent soldier1Team1Mesh{MeshManager::instance()->init_mesh(soldier1Team1Sprite.width, soldier1Team1Sprite.height, soldier1Team1Sprite.sprite_size.x, soldier1Team1Sprite.sprite_size.y,
                                                                       soldier1Team1Sprite.sprite_index.x,  soldier1Team1Sprite.sprite_index.y, 0)
    };
    ecsManager.addComponent<MeshComponent>(soldier1Team1, soldier1Team1Mesh);
    float soldier_radius = soldier1Team1Sprite.width/2 * 0.08f;
    float sb_width = soldier1Team1Sprite.width * 0.08f*0.8f;
    float sb_height = soldier1Team1Sprite.height * 0.08f*0.8f;
    ecsManager.addComponent(soldier1Team1, C_Collision{
            CollisionLayer::Enemy,
            soldier_radius,
            {sb_width, sb_height}
    });

    // Team 2 Soldiers
    Entity soldier1Team2 = ecsManager.createEntity();
    ecsManager.addComponent<Team>(soldier1Team2, Team{ TeamType::PLAYER2 });
    pos = tilemap->get_random_free_tile_position(MazeRegion::PLAYER2);
    ecsManager.addComponent<Transform>(soldier1Team2, Transform{
            pos,
            pos,
            {0.09f * 5 / 7, 0.09f},
            pos

    });
    Effect soldier1Team2Effect{};
    soldier1Team2Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(soldier1Team2, soldier1Team2Effect);
    Sprite soldier1Team2Sprite = { textures_path("blue_soldier_sprite_sheet-01.png") };
    TextureManager::instance()->load_from_file(soldier1Team2Sprite);
    soldier1Team2Sprite.sprite_index = { 0 , 0 };
    soldier1Team2Sprite.sprite_size = { soldier1Team2Sprite.width / 7.0f , soldier1Team2Sprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(soldier1Team2, soldier1Team2Sprite);
    MeshComponent soldier1Team2Mesh{MeshManager::instance()->init_mesh(soldier1Team2Sprite.width, soldier1Team2Sprite.height, soldier1Team2Sprite.sprite_size.x, soldier1Team2Sprite.sprite_size.y,
                                                                       soldier1Team2Sprite.sprite_index.x, soldier1Team2Sprite.sprite_index.y, 0)};
    ecsManager.addComponent<MeshComponent>(soldier1Team2, soldier1Team2Mesh);

    ecsManager.addComponent(soldier1Team2, C_Collision{
            CollisionLayer::Enemy,
            soldier_radius,
            {sb_height, sb_height}
    });

    // CASTLE 1
    Entity castle1 = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(castle1, Transform{
            { 120.f, m_screen_size.y / 2 },
            { 120.f, m_screen_size.y / 2 },
            {0.5f, 0.5f},
            { 120.f, m_screen_size.y / 2 }
    });
    ecsManager.addComponent<Team>(castle1, Team{ TeamType::PLAYER1 });
    Effect castle1Effect{};
    castle1Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(castle1, castle1Effect);
    Sprite castle1Sprite = { textures_path("castle/CaptureTheCastle_castle_red.png") };
    TextureManager::instance()->load_from_file(castle1Sprite);
    ecsManager.addComponent<Sprite>(castle1, castle1Sprite);
    MeshComponent castle1Mesh{MeshManager::instance()->init_mesh(castle1Sprite.width, castle1Sprite.height)};
    ecsManager.addComponent<MeshComponent>(castle1, castle1Mesh);
    float castleWidth = castle1Sprite.width * 0.2f;
    float castleHeight = castle1Sprite.height * 0.2f;
    ecsManager.addComponent(castle1, C_Collision{
            CollisionLayer::Castle,
            0,
            {castleWidth, castleHeight}
    });

    // CASTLE 2
    Entity castle2 = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(castle2, Transform{
            {m_screen_size.x - 120.f, m_screen_size.y / 2},
            {m_screen_size.x - 120.f, m_screen_size.y / 2},
            {0.5f, 0.5f},
            {m_screen_size.x - 120.f, m_screen_size.y / 2}
    });
    ecsManager.addComponent<Team>(castle2, Team{ TeamType::PLAYER2 });
    Effect castle2Effect{};
    castle2Effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(castle2, castle2Effect);
    Sprite castle2Sprite = { textures_path("castle/CaptureTheCastle_castle_blue.png") };
    TextureManager::instance()->load_from_file(castle2Sprite);
    ecsManager.addComponent<Sprite>(castle2, castle2Sprite);
    MeshComponent castle2Mesh{MeshManager::instance()->init_mesh(castle2Sprite.width, castle2Sprite.height)};
    ecsManager.addComponent<MeshComponent>(castle2, castle2Mesh);
    ecsManager.addComponent(castle2, C_Collision{
            CollisionLayer::Castle,
            0,
            {castleWidth, castleHeight}
    });

    // PLAYER 1
    Entity player1 = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(player1, Transform{
            { 120.f, m_screen_size.y / 2 + 130.f },
//        { 120.f - castleWidth, m_screen_size.y / 2 - castleHeight},  debugging purpose
            { 120.f, m_screen_size.y / 2 + 130.f },
            {0.1f * 5 / 7, 0.1f},
            { 120.f, m_screen_size.y / 2 + 130.f }
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
    MeshComponent player1Mesh{MeshManager::instance()->init_mesh(player1Sprite.width, player1Sprite.height,
                                                                 player1Sprite.sprite_size.x, player1Sprite.sprite_size.y, player1Sprite.sprite_index.x, player1Sprite.sprite_index.y, 0)
    };
    ecsManager.addComponent<MeshComponent>(player1, player1Mesh);
    float player_radius = player1Sprite.width/2 * 0.09f;
    float player_bounding_width =player1Sprite.width*0.09f*0.8f;
    float player_bounding_height =player1Sprite.height*0.09f*0.8f;
    ecsManager.addComponent(player1, C_Collision{
            CollisionLayer::PLAYER1,
            player_radius,
            {player_bounding_width, player_bounding_height}
    });

    // PLAYER 2
    Entity player2 = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(player2, Transform{
            { m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
            //            { 120.f, m_screen_size.y / 2}, debugging purpose
            { m_screen_size.x - 120.f, m_screen_size.y / 2 + 130.f },
            {0.1f * 5 / 7, 0.1f},
            { 120.f, m_screen_size.y / 2 + 130.f }
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
    MeshComponent player2Mesh{MeshManager::instance()->init_mesh(player2Sprite.width, player2Sprite.height,
                                                                 player2Sprite.sprite_size.x, player2Sprite.sprite_size.y, player2Sprite.sprite_index.x, player2Sprite.sprite_index.y, 0)
    };
    ecsManager.addComponent<MeshComponent>(player2, player2Mesh);
    ecsManager.addComponent(player2, C_Collision{
            CollisionLayer::PLAYER2,
            player_radius,
            {player_bounding_width, player_bounding_height}
    });

    banditAISystem->init(tilemap, player1, player2);

    // ITEM BOARD (PLAYER 1)
    Entity player1_board = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(player1_board, Transform{
            { 180.f, 65 },
            { 180.f, 65 },
            {0.5f, 0.5f},
            { 180.f, 65 }
    });
    ecsManager.addComponent<Team>(player1_board, Team{TeamType::PLAYER1});
    Effect player1BoardEffect{};
    player1BoardEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(player1_board, player1BoardEffect);
    Sprite player1BoardSprite = {textures_path("ui/CaptureTheCastle_player_tile_red.png")};
    TextureManager::instance()->load_from_file(player1BoardSprite);
    ecsManager.addComponent<Sprite>(player1_board, player1BoardSprite);
    MeshComponent player1BoardMesh{MeshManager::instance()->init_mesh(player1BoardSprite.width, player1BoardSprite.height)};
    ecsManager.addComponent<MeshComponent>(player1_board, player1BoardMesh);

    // ITEM BOARD (PLAYER 2)
    Entity player2_board = ecsManager.createEntity();
    ecsManager.addComponent<Transform>(player2_board, Transform{
            { m_screen_size.x - 180.f, 65.f },
            { m_screen_size.x - 180.f, 65.f },
            {0.5f, 0.5f},
            { m_screen_size.x - 180.f, 65.f }
    });
    ecsManager.addComponent<Team>(player2_board, Team{TeamType::PLAYER2});
    Effect player2BoardEffect{};
    player2BoardEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(player2_board, player2BoardEffect);
    Sprite player2BoardSprite = {textures_path("ui/CaptureTheCastle_player_tile_blue.png")};
    TextureManager::instance()->load_from_file(player2BoardSprite);
    ecsManager.addComponent<Sprite>(player2_board, player2BoardSprite);
    MeshComponent player2BoardMesh{MeshManager::instance()->init_mesh(player2BoardSprite.width, player2BoardSprite.height)};
    ecsManager.addComponent<MeshComponent>(player2_board, player2BoardMesh);

    movementSystem->setScreenSize(m_screen_size);
    //HELP BUTTON
    help_btn.init(m_screen_size);

    // Help Window Initialization
    help_window.init(m_screen_size);

    // Winner's Window Initialization
    win_window.init(m_screen_size);

    //--------------------------------------------------------------------------
    // Render all the tiles once to the screen texture

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

    // Render each tile to our screen texture
    tilemap->draw_all_tiles(m_world->get_projection_2d());

    return true;
}

bool Game::update(float elapsed_ms) {
    if (currState == GameState::NORMAL) {
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
    help_btn.draw(projection_2D);

    if (currState == GameState::HELP) {
        help_window.draw(projection_2D);
    }

    if (currState == GameState::WIN) {
        win_window.draw(projection_2D);
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
        default:
            break;
    }
    if (action == GLFW_PRESS && k != InputKeys::DEFAULT) {
        ecsManager.publish(new InputKeyEvent(k));
    } else if (action == GLFW_RELEASE && k != InputKeys::DEFAULT) {
        ecsManager.publish(new KeyReleaseEvent(k));
    }
}

void Game::on_mouse_click(GLFWwindow *pWindow, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (currState == GameState::NORMAL && help_btn.mouseOnButton({(float) xpos, (float) ypos })) {
            currState = GameState :: HELP;
        } else if (currState == GameState::HELP) {
            switch (help_window.checkButtonClicks({ (float) xpos, (float) ypos }))
            {
                case (ButtonActions::CLOSE):
                    currState = GameState :: NORMAL;
                    break;
                case (ButtonActions::HOWTOPLAY):
                    break;
                default:
                    break;
            }
        } else if (currState == GameState::WIN) {
            switch (win_window.checkButtonClicks({ (float) xpos, (float) ypos }))
            {
                case (ButtonActions::MAIN):
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
        }
    }
}

void Game::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    if (currState == GameState::NORMAL) {
        help_btn.onHover(help_btn.mouseOnButton({ (float)xpos, (float) ypos }));
    }
    if (currState == GameState::HELP) {
        help_window.checkButtonHovers({ (float) xpos, (float) ypos });
    }
    if (currState == GameState::WIN) {
        win_window.checkButtonHovers({ (float) xpos, (float) ypos });
    }
}

void Game::reset() {
    ecsManager.reset();
    std::cout << "ECS reset" << std::endl;
    tilemap->destroy();
    std::cout << "TileMap reset" << std::endl;
    help_btn.destroy();
    std::cout << "Help button destroyed" << std::endl;
    help_window.destroy();
    std::cout << "Help window destroyed" << std::endl;
    win_window.destroy();
    std::cout << "Win window destroyed" << std::endl;
    std::cout << "Reinitializing game state" << std::endl;
    init_state(m_world);
}

void Game::winListener(WinEvent *winEvent) {
    Team winningTeam = ecsManager.getComponent<Team>(winEvent->player);
    win_window.setWinTeam(winningTeam.assigned);
    currState = GameState :: WIN;
}

void Game::destroy() {
    glDeleteFramebuffers(1, &m_frame_buffer);
    if (m_background_music != nullptr)
        Mix_FreeMusic(m_background_music);
    ecsManager.reset();
    tilemap->destroy();
    help_btn.destroy();
    help_window.destroy();
    win_window.destroy();
}

Game::~Game() {
}


