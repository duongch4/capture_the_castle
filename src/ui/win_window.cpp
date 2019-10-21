
#include "win_window.hpp"
#include "ecs/events.hpp"

void WinWindow::init(vec2 screen_size) {
    // Initialize help button
    winBackground.init(screen_size);
    vec2 currPos = winBackground.get_position();
    winnerDp.init({currPos.x, currPos.y + 200});
}

void WinWindow::destroy() {
    winBackground.destroy();
}

void WinWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    winBackground.draw(projection);
}

ButtonActions WinWindow::checkButtonClicks(vec2 mouseloc) {
//    if (close_btn.mouseOnButton(mouseloc)) {
//        return ButtonActions::CLOSE;
//    } else {
//        return ButtonActions::NONE;
//    }
}

void WinWindow::checkButtonHovers(vec2 mouseloc) {
    //close_btn.onHover(close_btn.mouseOnButton(mouseloc));
}
