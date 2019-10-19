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

        float step = motion.speed * (ms / 1000);
        transform.position.x += motion.direction.x * step;
        transform.position.y += motion.direction.y * step;
    }
}
