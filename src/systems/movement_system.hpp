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
	void setScreenSize(const vec2& screen);
	void reset() override;

private:
	enum struct SPRITE_SHEET_DIR: int
	{
		DEFAULT = 0,
		UP = 1,
		LEFT = 2,
		RIGHT = 3,
		DOWN = 4 
	};
	vec2 screenSize;
	void set_bounding_window(Transform& transform);
	void set_sprite_motion(Sprite& sprite, Motion& motion);
	void set_sprite_direction(Motion& motion, Sprite& sprite);
};


#endif //CAPTURE_THE_CASTLE_MOVEMENTSYSTEM_HPP
