
#include "menu.hpp"
#include "game.hpp"

bool Menu::init_state(World *world) {
    m_world = world;
    background.init(m_world->get_screen_size());
    vec2 offset = {m_world->get_screen_size().x / 6, m_world->get_screen_size().y / 6};
    vec2 background_pos = background.get_position();
    quit_btn.init({background_pos.x, (float) (background_pos.y + (offset.y * 2.4))}, textures_path("ui/CaptureTheCastle_quit_btn.png"));
    quit_btn.setScale({0.8, 0.4});
    how_to_play_btn.init({background_pos.x - offset.x, (float) (background_pos.y + (offset.y * 1.5))}, textures_path("ui/CaptureTheCastle_how_to_play_btn.png"));
    how_to_play_btn.setScale({0.8, 0.4});
    new_game_btn.init({background_pos.x + offset.x, (float) (background_pos.y + (offset.y * 1.5))}, textures_path("ui/CaptureTheCastle_new_game_btn.png"));
    new_game_btn.setScale({0.8, 0.4});
    return true;
}

void Menu::on_key(int key, int action) {
    // no actions
}

void Menu::on_mouse_click(GLFWwindow *window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        switch (checkButtonClicks({ (float) xpos, (float) ypos })) {
            case ButtonActions::RESTART:
                m_world->set_state(new Game);
                break;
            case ButtonActions::QUIT:
                m_world->set_window_closed();
                break;
            case ButtonActions::HOWTOPLAY:
                break;
            case ButtonActions::NONE:
                break;
        }
    }
}

void Menu::on_mouse_move(GLFWwindow *window, double xpos, double ypos) {
    new_game_btn.onHover(new_game_btn.mouseOnButton({(float) xpos, (float) ypos}));
    quit_btn.onHover(quit_btn.mouseOnButton({(float) xpos, (float) ypos}));
    how_to_play_btn.onHover(how_to_play_btn.mouseOnButton({(float) xpos, (float) ypos}));
}

bool Menu::update(float elapsed_ms) {
    return false;
}

void Menu::draw() {
    background.draw(m_world->get_projection_2d());
    quit_btn.draw(m_world->get_projection_2d());
    new_game_btn.draw(m_world->get_projection_2d());
    how_to_play_btn.draw(m_world->get_projection_2d());
}

void Menu::reset() {
    // not needed
}

void Menu::destroy() {
    background.destroy();
    quit_btn.destroy();
    new_game_btn.destroy();
    how_to_play_btn.destroy();
}

ButtonActions Menu::checkButtonClicks(vec2 mouseloc) {
    if (new_game_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::RESTART;
    } else if (quit_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::QUIT;
    } else if (how_to_play_btn.mouseOnButton(mouseloc)){
        return ButtonActions::HOWTOPLAY;
    } else {
        return ButtonActions::NONE;
    }
}
