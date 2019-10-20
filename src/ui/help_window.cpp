
#include <texture_manager.hpp>
#include "help_window.hpp"

void HelpWindow::init(vec2 screen_size) {
    help_instructions.init(screen_size);
    vec2 currPos = help_instructions.get_position();
    vec2 box = help_instructions.get_bounding_box();
    close_btn.init({currPos.x + box.x/2, currPos.y + box.y/2 });
}

void HelpWindow::destroy() {
    close_btn.destroy();
    help_instructions.destroy();
}

void HelpWindow::draw(const mat3 &projection) {
    //draw all the buttons in window
    close_btn.draw(projection);
    help_instructions.draw(projection);
}
