#ifndef CAPTURE_THE_CASTLE_HOWTOPLAY_HPP
#define CAPTURE_THE_CASTLE_HOWTOPLAY_HPP

#include <world.hpp>
#include <ui/howtoplay_background.hpp>
#include <ui/play_instructions.hpp>
#include "state.hpp"

class HowToPlay : public State {
    bool init_state(World* world) override;
    void on_key(int key, int action) override;
    void on_mouse_click(GLFWwindow *window, int button, int action, int mods) override;
    void on_mouse_move(GLFWwindow *window, double xpos, double ypos) override;
    bool update(float elapsed_ms) override;
    void draw() override;
    void reset() override;
    void destroy() override;


private:
    ButtonActions checkButtonClicks(vec2 mouseloc);

    HowToPlayBackground background;
    PlayInstructions instructions;
    PopUpButton quit_btn;
    PopUpButton new_game_btn;
    PopUpButton main_menu_btn;
    World* m_world;
    Mix_Music* m_background_music;
    Mix_Chunk* m_click;
};


#endif //CAPTURE_THE_CASTLE_HOWTOPLAY_HPP
