
#include "win_window.hpp"
#include "ecs/events.hpp"

void WinWindow::init(vec2 screen_size) {
    // Initialize help button
    winBackground.init(screen_size);
    vec2 currPos = winBackground.get_position();
    winnerDp.init({currPos.x, currPos.y - 100});
    restart_btn.init({currPos.x, currPos.y + 75}, textures_path("ui/CaptureTheCastle_new_game_btn.png"));
    main_btn.init({currPos.x, currPos.y + 150}, textures_path("ui/CaptureTheCastle_main_menu_btn.png"));
    quit_btn.init({currPos.x, currPos.y + 225}, textures_path("ui/CaptureTheCastle_quit_btn.png"));
    m_click = Mix_LoadWAV(audio_path("capturethecastle_button_click.wav"));
}

void WinWindow::destroy() {
    winBackground.destroy();
    winnerDp.destroy();
    restart_btn.destroy();
    main_btn.destroy();
    quit_btn.destroy();
}

void WinWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    winBackground.draw(projection);
    winnerDp.draw(projection);
    restart_btn.draw(projection);
    main_btn.draw(projection);
    quit_btn.draw(projection);
}

ButtonActions WinWindow::checkButtonClicks(vec2 mouseloc) {
    if (restart_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::RESTART;
    } else if (main_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::MAIN;
    } else if (quit_btn.mouseOnButton(mouseloc)) {
        Mix_PlayChannel(-1, m_click, 0);
        return ButtonActions::QUIT;
    } else {
        return ButtonActions::NONE;
    }
}

void WinWindow::checkButtonHovers(vec2 mouseloc) {
    restart_btn.onHover(restart_btn.mouseOnButton(mouseloc));
    main_btn.onHover(main_btn.mouseOnButton(mouseloc));
    quit_btn.onHover(quit_btn.mouseOnButton(mouseloc));
}

void WinWindow::setWinTeam(TeamType team) {
    winnerDp.setWinnerDp(team);
}
