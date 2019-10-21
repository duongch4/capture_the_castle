
#include "help_window.hpp"
#include "ecs/events.hpp"

void HelpWindow::init(vec2 screen_size) {
    // Initialize help button
    help_instructions.init(screen_size);
    vec2 currPos = help_instructions.get_position();
    close_btn.init({currPos.x + 410, currPos.y - 280});
}

void HelpWindow::destroy() {
    close_btn.destroy();
    help_instructions.destroy();
}

void HelpWindow::draw(const mat3 &projection) {
    //draw all the UI objects of window
    help_instructions.draw(projection);
    close_btn.draw(projection);
}

ButtonActions HelpWindow::checkButtonClicks(vec2 mouseloc) {
    if (close_btn.mouseOnButton(mouseloc)) {
        return ButtonActions::CLOSE;
    }
}

void HelpWindow::checkButtonHovers(vec2 mouseloc) {
    close_btn.onHover(close_btn.mouseOnButton(mouseloc));
}
