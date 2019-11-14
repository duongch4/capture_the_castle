
#include "help_window.hpp"
#include "ecs/events.hpp"

void HelpWindow::init(vec2 screen_size) {
    // Initialize help button
    curr_state = CONTROLS;
    background.init(screen_size);
    vec2 currPos = background.get_position();
    close_btn.init({currPos.x + 410, currPos.y - 280});
    instructions.init({currPos.x, currPos.y + 22 }, textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
    instructions.setScale({0.275, 0.275});
    how_to_play_btn.init({currPos.x + 200, currPos.y + 220},
                          textures_path("ui/CaptureTheCastle_how_to_play_btn.png"));

    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
}

void HelpWindow::destroy() {
    close_btn.destroy();
    background.destroy();
    how_to_play_btn.destroy();
    instructions.destroy();
}

void HelpWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    background.draw(projection);
    close_btn.draw(projection);
    instructions.draw(projection);
    if (curr_state == CONTROLS) {
        how_to_play_btn.draw(projection);
    }
}

ButtonActions HelpWindow::checkButtonClicks(vec2 mouseloc) {
    if (close_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::CLOSE;
    } else if (how_to_play_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::HOWTOPLAY;
    } else {
        return ButtonActions::NONE;
    }
}

void HelpWindow::checkButtonHovers(vec2 mouseloc) {
    close_btn.onHover(close_btn.mouseOnButton(mouseloc));
    how_to_play_btn.onHover(how_to_play_btn.mouseOnButton(mouseloc));
}

void HelpWindow::showHowToPlay() {
    curr_state = HOWTOPLAY;
    instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_how_to_play_instructions.png"));
    instructions.setScale({0.2, 0.2});
}

void HelpWindow::resetWindow() {
    curr_state = CONTROLS;
    instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
    instructions.setScale({0.275, 0.275});
}


