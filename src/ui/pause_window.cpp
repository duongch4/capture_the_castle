
#include "pause_window.hpp"
#include "ecs/events.hpp"

void PauseWindow::init(vec2 screen_size) {
    // Initialize help button
    background.init(screen_size, textures_path("ui/CaptureTheCastle_pause_screen.png"));
    vec2 currPos = background.get_position();
    image.init({currPos.x, currPos.y - 100}, textures_path("ui/CaptureTheCastle_paused_img.png"));
    close_btn.init({currPos.x + 410, currPos.y - 280});
    restart_btn.init({currPos.x, currPos.y + 75}, textures_path("ui/CaptureTheCastle_new_game_btn.png"));
    main_btn.init({currPos.x, currPos.y + 150}, textures_path("ui/CaptureTheCastle_main_menu_btn.png"));
    quit_btn.init({currPos.x, currPos.y + 225}, textures_path("ui/CaptureTheCastle_quit_btn.png"));
    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
}

void PauseWindow::destroy() {
    background.destroy();
    image.destroy();
    close_btn.destroy();
    restart_btn.destroy();
    main_btn.destroy();
    quit_btn.destroy();
    if (m_click != nullptr)
        Mix_FreeChunk(m_click);
}

void PauseWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    background.draw(projection);
    image.draw(projection);
    close_btn.draw(projection);
    restart_btn.draw(projection);
    main_btn.draw(projection);
    quit_btn.draw(projection);
}

ButtonActions PauseWindow::checkButtonClicks(vec2 mouseloc) {
    if (restart_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::RESTART;
    } else if (main_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::MAIN;
    } else if (quit_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::QUIT;
    } else if (close_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::CLOSE;
    } else {
        return ButtonActions::NONE;
    }
}

void PauseWindow::checkButtonHovers(vec2 mouseloc) {
    restart_btn.onHover(restart_btn.mouseOnButton(mouseloc));
    main_btn.onHover(main_btn.mouseOnButton(mouseloc));
    quit_btn.onHover(quit_btn.mouseOnButton(mouseloc));
    close_btn.onHover(close_btn.mouseOnButton(mouseloc));
}

