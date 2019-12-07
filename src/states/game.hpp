//
// Created by Owner on 2019-11-06.
//

#ifndef CAPTURE_THE_CASTLE_GAME_HPP
#define CAPTURE_THE_CASTLE_GAME_HPP


#include <world.hpp>
#include <mesh_manager.hpp>
#include "state.hpp"
#include "menu.hpp"
#include "components.hpp"
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
#include <systems/curve_movement_system.hpp>
#include <systems/item_effect_system.hpp>
#include <ui/countdown_timer.hpp>
#include <ui/pause_window.hpp>
#include <ui/soldier_setup_window.hpp>

class Game : public State {
public:
    Game();
    ~Game();
    bool init_state(World* world) override;
    void on_key(int key, int action) override;
    void on_mouse_click(GLFWwindow *window, int button, int action, int mods) override;
    void on_mouse_move(GLFWwindow *window, double xpos, double ypos) override;

    bool update(float elapsed_ms) override;
    void draw() override;
    void reset() override;
    void destroy() override;

    void registerComponents();

    void registerItemBoards(const vec2& screen);
    void registerPlayers(std::vector<Entity>& players);
    void registerCastles();
    void registerItemBoard(const Transform& transform, const TeamType& team_type, const char* texture_path);
    Entity registerPlayer(const Transform& transform, const Motion& motion, const TeamType& team_type, const char* texture_path);
    void registerCastle(const Transform& transform, const TeamType& team_type, const char* texture_path);
	Entity registerBubble(Entity player, const char* texture_path);

    void registerBanditAiSystem();
    void registerBoxCollisionSystem();
    void registerCollisionSystem();
    void registerBanditSpawnSystem();
    void registerSpriteRenderSystem();
    void registerPlayerInputSystem();
    void registerMovementSystem(const vec2& screen);
    void registerCurveMovementSystem();
    void registerItemSpawnSystem();
    void registerItemBoardSystem();
    void registerItemEffectSystem();

    void renderTilesToScreenTexture();

private:
    // Audio
    Mix_Music* m_background_music;
    Mix_Chunk* m_click;

    // Screen size
    vec2 m_screen_size;

    // Tile map
    std::shared_ptr<Tilemap> tilemap;

    // Screen texture
    GLuint m_frame_buffer;
    Texture m_screen_tex;

    // Game UI
    HelpButton help_btn;
    HelpWindow help_window;
    WinWindow win_window;
    PauseWindow pause_window;
    SetupWindow setup_window;
    PlayInstructions p1SetUpInstructions;
    PlayInstructions p2SetUpInstructions;
    CountdownTimer timer;

    // Particle
    Firework firework;

    // ECS systems
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<PlayerInputSystem> playerInputSystem;
    std::shared_ptr<SpriteRenderSystem> spriteRenderSystem;
    std::shared_ptr<BanditSpawnSystem> banditSpawnSystem;
    std::shared_ptr<BanditAiSystem> banditAiSystem;
    std::shared_ptr<SoldierAiSystem> soldierAiSystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<BoxCollisionSystem> boxCollisionSystem;
    std::shared_ptr<CurveMovementSystem> curveMovementSystem;
    std::shared_ptr<ItemSpawnSystem> itemSpawnSystem;
    std::shared_ptr<ItemBoardSystem> itemBoardSystem;
    std::shared_ptr<ItemEffectSystem> itemEffectSystem;


    void winListener(WinEvent* winEvent);
	void flagListener(FlagEvent* flagEvent);
    bool init_game();

    //Game state
	enum GameState { START, HELP, WIN, NORMAL, PAUSE, SETUP , FLAG,};
    GameState currState;
	GameState oldState;
    CollisionLayer winner;
    World* m_world;

    void registerSoldierAiSystem();
};


#endif //CAPTURE_THE_CASTLE_GAME_HPP
