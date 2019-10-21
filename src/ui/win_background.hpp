#pragma once

#include "common.hpp"
#include "components.hpp"

class WinBackground
{

public:
    void init(vec2 screen_size);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

private:
    Mesh mesh{};
    Effect effect{};
    Transform transform;
    Texture winbgSprite;
    mat3 out;
};