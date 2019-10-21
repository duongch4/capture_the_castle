//
// Created by Tianyan Zhu on 2019-10-17.
//

#include "collision_system.hpp"
#include "math.h"
#include <ecs/ecs_manager.hpp>


extern ECSManager ecsManager;

void CollisionSystem::init() {
    ecsManager.subscribe(this, &CollisionSystem::collisionListener);
}

void CollisionSystem::checkCollision() {
    for(auto const& entity1: entities) {
        for(auto const& entity2: entities) {
            auto& e1_collision = ecsManager.getComponent<C_Collision>(entity1);
            auto& e2_collision = ecsManager.getComponent<C_Collision>(entity2);

            auto& e1_transform = ecsManager.getComponent<Transform>(entity1);
            auto& e2_transform = ecsManager.getComponent<Transform>(entity2);
            if (e1_collision.layer < e2_collision.layer){
                if (distance(e1_transform.position, e2_transform.position) < (e1_collision.radius + e2_collision.radius)){
                    ecsManager.publish(new CollisionEvent(entity1, entity2));
                }
            }
        }
    }
}

void CollisionSystem::update() {
    while (collision_queue.size() > 0) {
        std::pair<Entity, Entity> collision_pair = collision_queue.front();
        Entity e1 = collision_pair.first;
        Entity e2 = collision_pair.second;

        TeamType e1_team = ecsManager.getComponent<Team>(e1).assigned;
        TeamType e2_team = ecsManager.getComponent<Team>(e2).assigned;

        if (e1_team == e2_team){
            collision_queue.pop();
            break;
        } else {
            CollisionLayer e1_layer = ecsManager.getComponent<C_Collision>(e1).layer;
            CollisionLayer e2_layer = ecsManager.getComponent<C_Collision>(e2).layer;

            auto& e1_transform = ecsManager.getComponent<Transform>(e1);
            auto& e2_transform = ecsManager.getComponent<Transform>(e2);
//           auto& e1_motion = ecsManager.getComponent<Motion>(e1);
//           auto& e2_motion = ecsManager.getComponent<Motion>(e2);
            MazeRegion region = Tilemap::get_region(e1_transform.position.x, e1_transform.position.y);

            if (e1_layer == CollisionLayer::PLAYER1 & e2_layer == CollisionLayer::PLAYER2){
                switch(region){
                    case MazeRegion::PLAYER1:
                        e2_transform.position = e2_transform.init_position;
                        break;
                    case MazeRegion::PLAYER2:
                        e1_transform.position = e1_transform.init_position;
                        break;
                    case MazeRegion::BANDIT:
                        break;
                }
            } else if (e2_layer == CollisionLayer::Enemy){
                switch(region){
                    case MazeRegion::PLAYER1:
                        if (e1_team == TeamType::PLAYER2){
                            e1_transform.position = e1_transform.init_position;
                        }
                        break;
                    case MazeRegion::PLAYER2:
                        if (e1_team == TeamType::PLAYER1){
                            e1_transform.position = e1_transform.init_position;
                        }
                        break;
                    case MazeRegion::BANDIT:
                        e1_transform.position = e1_transform.init_position;
                        break;
                }
            }
            collision_queue.pop();
        }
    }

}

float CollisionSystem::distance(vec2 e1, vec2 e2) {
    float dx = e1.x - e2.x;
    float dy = e1.y - e2.y;
    return sqrt(dx*dx + dy*dy);
}

void CollisionSystem::collisionListener(CollisionEvent* collisionEvent) {
    collision_queue.push(std::make_pair(collisionEvent->e1, collisionEvent->e2));
    std::pair<Entity, Entity> temp = collision_queue.front();
}