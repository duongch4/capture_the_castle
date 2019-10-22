//
// Created by Tianyan Zhu on 2019-10-17.
//

#ifndef CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <components.hpp>
#include <ecs/events.hpp>
#include <tilemap.hpp>

extern ECSManager ecsManager;

class CollisionSystem  : public System {

public:
    void init();
    void checkCollision();
    void update();

private:
    void collisionListener(CollisionEvent* collisionEvent);
    bool collideWithCastle(Entity player, Entity castle);
    float distance(vec2 e1, vec2 e2);
    std::queue<std::pair<Entity, Entity>> collision_queue;
};


#endif //CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP