#pragma once

#include "common.hpp"
#include "components.hpp"
#include "win_background.hpp"
#include "ecs/events.hpp"
#include "win_winnerDp.hpp"

class WinWindow
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    ButtonActions checkButtonClicks(vec2 mouseloc);

    void checkButtonHovers(vec2 mouseloc);

private:
    WinnerDp winnerDp;
    WinBackground winBackground;
};
