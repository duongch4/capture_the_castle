#pragma once

#include <SDL_mixer.h>
#include "common.hpp"
#include "components.hpp"
#include "popup_background.hpp"
#include "ecs/events.hpp"
#include "popUpImg.hpp"
#include "popupbtn.hpp"

class WinWindow
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    ButtonActions checkButtonClicks(vec2 mouseloc);

    void checkButtonHovers(vec2 mouseloc);

    void setWinTeam(TeamType team);

private:
    PopUpImage winnerDp;
    PopUpButton restart_btn;
    PopUpButton main_btn;
    PopUpButton quit_btn;
    PopUpBackground winBackground;
    Mix_Chunk* m_click;
};
