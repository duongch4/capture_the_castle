#pragma once

#include "common.hpp"
#include "components.hpp"

class PopUpButton
{

public:
    void init(vec2 pos, const char* texturePath);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

    bool mouseOnButton(vec2 mouseloc);

    void onHover(bool isHovering);

    void setScale(vec2 scale);

private:
    MeshComponent mesh{};
    Effect effect{};
    Transform transform;
    Texture popupBtnSprite;
    mat3 out;
    int currIndex;
};