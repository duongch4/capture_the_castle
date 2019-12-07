#pragma once

#include <effect_manager.hpp>
#include "common.hpp"
#include "components.hpp"

class PopUpImage
{

public:
    void init(vec2 pos, const char* texture_path);

    void destroy();

    void draw(const mat3& projection);

    void setWinnerDp(TeamType team);

private:
    MeshComponent mesh{};
    Effect effect{};
    Transform transform;
    Texture imageSprite;
    mat3 out;
};