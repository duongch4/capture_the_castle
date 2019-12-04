#pragma once

#include <SDL_mixer.h>
#include "common.hpp"
#include "components.hpp"
#include "popup_background.hpp"
#include "ecs/events.hpp"
#include "win_winnerDp.hpp"
#include "popupbtn.hpp"
#include "closebtn.hpp"

class PauseWindow
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    ButtonActions checkButtonClicks(vec2 mouseloc);

    void checkButtonHovers(vec2 mouseloc);

private:
    CloseButton close_btn;
    PopUpButton restart_btn;
    PopUpButton main_btn;
    PopUpButton quit_btn;
    PopUpBackground background;
    Mix_Chunk* m_click;
};
