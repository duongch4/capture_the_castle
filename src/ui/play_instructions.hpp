#pragma once

#include "common.hpp"
#include "components.hpp"

// Instructions class
class PlayInstructions
{

public:
    void init(vec2 pos, const char* texturePath);

    void destroy();

    void draw(const mat3& projection);

    vec2 get_position();

    vec2 get_bounding_box();

    void loadNewInstruction(const char* texturePath);

    void setScale(vec2 scale);

private:
    MeshComponent mesh{};
    Effect effect{};
    Transform transform;
    Texture helpInstrSprite;
    mat3 out;
};