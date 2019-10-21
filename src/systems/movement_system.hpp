//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP
#define CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP


#include <ecs/common_ecs.hpp>

class MovementSystem : public System {
public:
    void init();
    void update(float ms);
    void setScreenSize(vec2 screen);

private:
    vec2 screenSize;
};


#endif //CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP
