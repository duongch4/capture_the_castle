#pragma once

#include <effect_manager.hpp>
#include "common.hpp"
#include "components.hpp"

class PopUpBackground
{

public:
    void init(vec2 screen_size, const char* texturePath);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

private:
    MeshComponent mesh{};
    Effect effect{};
    Transform transform;
    Texture backgroundSprite;
    mat3 out;
};