//
// Created by Owner on 2019-11-11.
//

#ifndef CAPTURE_THE_CASTLE_MENU_HPP
#define CAPTURE_THE_CASTLE_MENU_HPP

#include <world.hpp>
#include "state.hpp"

class Menu : public State {
    bool init_state(World* world) override;
    void on_key(int key, int action) override;
    void on_mouse_click(GLFWwindow *window, int button, int action, int mods) override;
    void on_mouse_move(GLFWwindow *window, double xpos, double ypos) override;
    bool update(float elapsed_ms) override;
    void draw() override;
    void reset() override;
    void destroy() override;
};


#endif //CAPTURE_THE_CASTLE_MENU_HPP
