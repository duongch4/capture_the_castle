//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_RENDER_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_RENDER_SYSTEM_HPP


#include <ecs/common_ecs.hpp>
#include <common.hpp>

class Effect;

class SpriteRenderSystem : public System {
public:
    void init();
    void draw(mat3 projection);

private:
    mat3 out{};
};


#endif //CAPTURE_THE_CASTLE_RENDER_SYSTEM_HPP
