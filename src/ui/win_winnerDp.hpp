#pragma once

#include "common.hpp"
#include "components.hpp"

class WinnerDp
{

public:
    void init(vec2 pos);

    void destroy();

    void draw(const mat3& projection);

    void setWinnerDp(TeamType team);

private:
    Mesh mesh{};
    Effect effect{};
    Transform transform;
    Texture winnerDpSprite;
    mat3 out;
};