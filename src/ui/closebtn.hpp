#pragma once


#include <effect_manager.hpp>
#include "common.hpp"
#include "components.hpp"

// Close Button class
class CloseButton
{

public:
    void init(vec2 pos);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

    bool mouseOnButton(vec2 mouseloc);

    void onHover(bool isHovering);

private:
    MeshComponent mesh{};
    Effect effect{};
    Transform transform;
    Texture closebtn_texture;
    mat3 out;
    int currIndex;
};