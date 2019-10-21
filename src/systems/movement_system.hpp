//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP
#define CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP


#include <ecs/common_ecs.hpp>

class MovementSystem : public System
{
public:
    void init();
    void update(float ms);
private:
	enum struct SPRITE_SHEET_DIR: int
	{
		DEFAULT = 0,
		UP = 1,
		LEFT = 2,
		RIGHT = 3,
		DOWN = 4 
	};
    void setScreenSize(vec2 screen);

private:
    vec2 screenSize;
};


#endif //CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP
