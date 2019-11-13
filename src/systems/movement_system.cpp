//
// Created by Owner on 2019-10-12.
//

#include "ecs/ecs_manager.hpp"
#include "components.hpp"
#include "movement_system.hpp"


#include <iostream>
#include <mesh_manager.hpp>

extern ECSManager ecsManager;

void MovementSystem::init() {

}

void MovementSystem::update(float ms) {
    for (auto const &entity: entities) {
        auto &motion = ecsManager.getComponent<Motion>(entity);
        auto &transform = ecsManager.getComponent<Transform>(entity);
        auto &sprite = ecsManager.getComponent<Sprite>(entity);
        auto &mesh = ecsManager.getComponent<MeshComponent>(entity);
        float step = motion.speed * (ms / 1000);
        transform.old_position = transform.position;
		set_sprite_motion(sprite, motion);
		set_sprite_direction(motion, sprite);

		MeshManager::instance().update_sprite(mesh.id,
                                               sprite.width, sprite.height,
                                               (int)sprite.sprite_size.x, (int)sprite.sprite_size.y,
                                               (int)sprite.sprite_index.x, (int)sprite.sprite_index.y, 0);

        transform.position.x += motion.direction.x * step;
        transform.position.y += motion.direction.y * step;

		set_bounding_window(transform);
    }
}

void MovementSystem::set_bounding_window(Transform& transform)
{
	const float offset_x = 100.f;
	const float offset_y = 80.f;

	if (transform.position.x > (screenSize.x - offset_x))
	{
		transform.position.x = screenSize.x - offset_x;
	}
	if (transform.position.x < offset_x)
	{
		transform.position.x = offset_x;
	}
	if (transform.position.y >(screenSize.y - offset_y))
	{
		transform.position.y = screenSize.y - offset_y;
	}
	if (transform.position.y < 2 * offset_y)
	{
		transform.position.y = 2 * offset_y;

	}
}

void MovementSystem::set_sprite_motion(Sprite& sprite, Motion& motion)
{
	if (sprite.sprite_size.x > 0 && (motion.direction.x != 0 || motion.direction.y != 0))
	{
		sprite.sprite_index.x++;
		sprite.sprite_index.x = (float)((int)sprite.sprite_index.x % (int)sprite.sprite_size.x);
	}
}

void MovementSystem::set_sprite_direction(Motion& motion, Sprite& sprite)
{
	if (motion.direction.x < 0)
	{
		sprite.sprite_index.y = (int)SPRITE_SHEET_DIR::LEFT;
	}
	else if (motion.direction.x > 0)
	{
		sprite.sprite_index.y = (int)SPRITE_SHEET_DIR::RIGHT;
	}
	else if (motion.direction.y < 0)
	{
		sprite.sprite_index.y = (int)SPRITE_SHEET_DIR::UP;
	}
	else if (motion.direction.y > 0)
	{
		sprite.sprite_index.y = (int)SPRITE_SHEET_DIR::DOWN;
	}
	else
	{
		sprite.sprite_index.y = (int)SPRITE_SHEET_DIR::DEFAULT;
	}
}

void MovementSystem::setScreenSize(const vec2& screen) {
    this->screenSize.x = screen.x;
    this->screenSize.y = screen.y;
}

void MovementSystem::reset() {
	this->entities.clear();

}
