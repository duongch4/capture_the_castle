#pragma once

#include "common.hpp"
#include "components.hpp"
#include "closebtn.hpp"
#include "help_instructions.hpp"

class HelpWindow
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

private:
    CloseButton close_btn;
    HelpInstructions help_instructions;
};
