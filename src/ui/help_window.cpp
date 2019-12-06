
#include "help_window.hpp"
#include "ecs/events.hpp"

void HelpWindow::init(vec2 screen_size) {
    // Initialize help button
    curr_state = CONTROLS;
    background.init(screen_size, textures_path("ui/CaptureTheCastle_help_screen.png"));
    vec2 currPos = background.get_position();
    close_btn.init({currPos.x + 410, currPos.y - 280});
    instructions.init({currPos.x, currPos.y + 22 }, textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
    instructions.setScale({0.275, 0.275});
    how_to_play_btn.init({currPos.x + 200, currPos.y + 220},
                          textures_path("ui/CaptureTheCastle_how_to_play_btn.png"));
    controls_btn.init({currPos.x + 200, currPos.y + 220},
                         textures_path("ui/CaptureTheCastle_controls_btn.png"));

    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
    if (m_click == nullptr)
        fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
                audio_path("capturethecastle_button_click.wav"));
}

void HelpWindow::destroy() {
    close_btn.destroy();
    background.destroy();
    how_to_play_btn.destroy();
    controls_btn.destroy();
    instructions.destroy();
    if (m_click != nullptr)
        Mix_FreeChunk(m_click);
}

void HelpWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    background.draw(projection);
    close_btn.draw(projection);
    instructions.draw(projection);
    if (curr_state == HOWTOPLAY) {
        controls_btn.draw(projection);
    } else {
        how_to_play_btn.draw(projection);
    }
}

ButtonActions HelpWindow::checkButtonClicks(vec2 mouseloc) {
    if (close_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::CLOSE;
    } else if (curr_state == State::CONTROLS && how_to_play_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        showHowToPlay();
        return ButtonActions::NONE;
    } else if (curr_state == State::HOWTOPLAY && controls_btn.mouseOnButton(mouseloc)){
        Mix_PlayChannel(-1, m_click, 0);
        showControls();
        return ButtonActions::NONE;
    } else {
        return ButtonActions::NONE;
    }
}

void HelpWindow::checkButtonHovers(vec2 mouseloc) {
    close_btn.onHover(close_btn.mouseOnButton(mouseloc));
    if (curr_state == HOWTOPLAY) {
        controls_btn.onHover(controls_btn.mouseOnButton(mouseloc));
    } else {
        how_to_play_btn.onHover(how_to_play_btn.mouseOnButton(mouseloc));
    }
}

void HelpWindow::showHowToPlay() {
    curr_state = HOWTOPLAY;
    instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_how_to_play_instructions.png"));
    instructions.setScale({0.2, 0.2});
    vec2 pos = background.get_position();
    instructions.setPosition({pos.x, pos.y - 22});
}

void HelpWindow::showControls() {
    curr_state = CONTROLS;
    instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
    instructions.setScale({0.275, 0.275});
    vec2 pos = background.get_position();
    instructions.setPosition({pos.x, pos.y + 22});
}

void HelpWindow::resetWindow() {
    curr_state = CONTROLS;
    instructions.loadNewInstruction(textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
    instructions.setScale({0.275, 0.275});
    vec2 pos = background.get_position();
    instructions.setPosition({pos.x, pos.y + 22});
}


