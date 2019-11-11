#include <ecs/common_ecs.hpp>
#include "menus_background.hpp"
#include "popupbtn.hpp"

class MenuWindow
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    ButtonActions checkButtonClicks(vec2 mouseloc);

    void checkButtonHovers(vec2 mouseloc);

private:
    PopUpButton load_btn;
    PopUpButton quit_btn;
    PopUpButton how_to_play_btn;
    PopUpButton new_game_btn;
    MenuBackground menuBackground;
};
