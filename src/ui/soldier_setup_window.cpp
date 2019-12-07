
#include "soldier_setup_window.hpp"
#include "ecs/events.hpp"

void SetupWindow::init(vec2 screen_size) {
    background.init(screen_size, textures_path("ui/CaptureTheCastle_help_screen.png"));
    vec2 currPos = background.get_position();
    instructions.init({currPos.x, currPos.y + 22 }, textures_path("ui/CaptureTheCastle_game_control_instructions.png"));
    instructions.setScale({0.275f, 0.275f});
    start_btn.init({currPos.x + 200, currPos.y + 220}, textures_path("ui/CaptureTheCastle_start_btn.png"));
    start_btn.setScale({0.7f, 0.35f });

    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
    if (m_click == nullptr)
        fprintf(stderr, "Failed to load sounds\n %s\n make sure the data directory is present",
                audio_path("capturethecastle_button_click.wav"));
}

void SetupWindow::destroy() {
    background.destroy();
    start_btn.destroy();
    instructions.destroy();
    if (m_click != nullptr)
        Mix_FreeChunk(m_click);
}

void SetupWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    background.draw(projection);
    instructions.draw(projection);
    start_btn.draw(projection);
}

ButtonActions SetupWindow::checkButtonClicks(vec2 mouseloc) {
    if (start_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::START;
    } else {
        return ButtonActions::NONE;
    }
}

void SetupWindow::checkButtonHovers(vec2 mouseloc) {
    start_btn.onHover(start_btn.mouseOnButton(mouseloc));
}


