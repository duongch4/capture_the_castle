#pragma once

#include "common.hpp"
#include "components.hpp"

// Help Button class
class HelpButton
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

    bool mouseOnButton(vec2 mouseloc);

    void onHover(bool isHovering);

private:
    Mesh mesh{};
    Effect effect{};
    Transform transform;
    Texture helpBtnSprite;
    mat3 out;
    int currIndex;
};