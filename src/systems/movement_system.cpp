//
// Created by Owner on 2019-10-12.
//

#include "ecs/ecs_manager.hpp"
#include "components.hpp"
#include "movement_system.hpp"

extern ECSManager ecsManager;

void MovementSystem::init() {

}

void MovementSystem::update(float ms) {
    for(auto const& entity: entities) {
        auto& motion = ecsManager.getComponent<Motion>(entity);
        auto& transform = ecsManager.getComponent<Transform>(entity);
		auto& sprite = ecsManager.getComponent<Sprite>(entity);
		auto& mesh = ecsManager.getComponent<Mesh>(entity);
        float step = motion.speed * (ms / 1000);
		if (sprite.sprite_size.x > 0 && (motion.direction.x != 0 || motion.direction.y != 0))
		{
			if (sprite.sprite_index.x < 6)
				sprite.sprite_index.x++;
			else
				sprite.sprite_index.x = 0;
			mesh.init(sprite.width, sprite.height, (int) sprite.sprite_size.x, (int) sprite.sprite_size.y, (int) sprite.sprite_index.x, (int) sprite.sprite_index.y, 0);
		}
        transform.position.x += motion.direction.x * step;
        transform.position.y += motion.direction.y * step;
    }
}
