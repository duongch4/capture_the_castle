//
// Created by Owner on 2019-11-06.
//

#ifndef CAPTURE_THE_CASTLE_GAME_HPP
#define CAPTURE_THE_CASTLE_GAME_HPP


#include <world.hpp>
#include "state.hpp"

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

private:
    // Audio
    Mix_Music* m_background_music;

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

    // ECS systems
    std::shared_ptr<MovementSystem> movementSystem;
    std::shared_ptr<PlayerInputSystem> playerInputSystem;
    std::shared_ptr<SpriteRenderSystem> spriteRenderSystem;
    std::shared_ptr<BanditSpawnSystem> banditSpawnSystem;
    std::shared_ptr<BanditAISystem> banditAISystem;
    std::shared_ptr<CollisionSystem> collisionSystem;
    std::shared_ptr<BoxCollisionSystem> boxCollisionSystem;
    std::shared_ptr<ItemSpawnSystem> itemSpawnSystem;

    void winListener(WinEvent* winEvent);
    bool init_game();

    //Game state
    enum GameState { HELP, WIN, NORMAL};
    GameState currState;
    CollisionLayer winner;

    World* m_world;
};


#endif //CAPTURE_THE_CASTLE_GAME_HPP
