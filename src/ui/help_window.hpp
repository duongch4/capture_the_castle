#pragma once

#include "common.hpp"
#include "components.hpp"
#include "closebtn.hpp"
#include "help_instructions.hpp"
#include "ecs/events.hpp"

class HelpWindow
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    ButtonActions checkButtonClicks(vec2 mouseloc);

    void checkButtonHovers(vec2 mouseloc);

private:
    CloseButton close_btn;
    HelpInstructions help_instructions;
};
