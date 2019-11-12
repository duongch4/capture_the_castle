
#include "menus_window.hpp"

void MenuWindow::init(vec2 screen_size) {
    menuBackground.init(screen_size);
    vec2 currPos = menuBackground.get_position();
    load_btn.init({currPos.x, currPos.y + 75}, textures_path("ui/CaptureTheCastle_load_game_btn.png"));
    quit_btn.init({currPos.x, currPos.y + 225}, textures_path("ui/CaptureTheCastle_quit_btn.png"));
    how_to_play_btn.init({currPos.x, currPos.y + 225}, textures_path("ui/CaptureTheCastle_how_to_play_btn.png"));
    new_game_btn.init({currPos.x, currPos.y + 75}, textures_path("ui/CaptureTheCastle_new_game_btn.png"));
}

void MenuWindow::destroy() {
    load_btn.destroy();
    quit_btn.destroy();
    how_to_play_btn.destroy();
    new_game_btn.destroy();
    menuBackground.destroy();
}

void MenuWindow::draw(const mat3 &projection) {
    menuBackground.draw(projection);
    load_btn.draw(projection);
    quit_btn.draw(projection);
    how_to_play_btn.draw(projection);
    new_game_btn.draw(projection);
}

ButtonActions MenuWindow::checkButtonClicks(vec2 mouseloc) {
    if (new_game_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::RESTART;
    } else if (load_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::LOAD;
    } else if (quit_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::QUIT;
    } else if (how_to_play_btn.mouseOnButton(mouseloc)){
        return ButtonActions::HOWTOPLAY;
    } else {
        return ButtonActions::NONE;
    }
}

void MenuWindow::checkButtonHovers(vec2 mouseloc) {
    new_game_btn.onHover(new_game_btn.mouseOnButton(mouseloc));
    load_btn.onHover(load_btn.mouseOnButton(mouseloc));
    quit_btn.onHover(quit_btn.mouseOnButton(mouseloc));
    how_to_play_btn.onHover(how_to_play_btn.mouseOnButton(mouseloc));
}
