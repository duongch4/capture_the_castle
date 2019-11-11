//
// Created by Owner on 2019-11-10.
//

#ifndef CAPTURE_THE_CASTLE_STATE_HPP
#define CAPTURE_THE_CASTLE_STATE_HPP

#include <world.hpp>
#include <GLFW/glfw3.h>

class World;

class State {
public:
    virtual bool init_state(World* world) = 0;
    virtual void on_key(int key, int action) = 0;
    virtual void on_mouse_click(GLFWwindow *window, int button, int action, int mods) = 0;
    virtual void on_mouse_move(GLFWwindow *window, double xpos, double ypos) = 0;
    virtual bool update(float elapsed_ms) = 0;
    virtual void draw() = 0;
    virtual void reset() = 0;
    virtual void destroy() = 0;
};
#endif //CAPTURE_THE_CASTLE_STATE_HPP
