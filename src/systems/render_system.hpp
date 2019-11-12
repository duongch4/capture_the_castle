//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_RENDER_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_RENDER_SYSTEM_HPP

#include <sstream>

#include <ecs/ecs_manager.hpp>
#include <ecs/common_ecs.hpp>
#include <common.hpp>
#include <components.hpp>
#include <texture_manager.hpp>

class SpriteRenderSystem : public System {
public:
    void init();
    void draw(mat3 projection);
    void reset() override;
    
private:
    mat3 out{};
};


#endif //CAPTURE_THE_CASTLE_RENDER_SYSTEM_HPP
