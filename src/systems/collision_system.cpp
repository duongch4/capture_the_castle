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
                if (e2_collision.layer == CollisionLayer :: Castle && (e1_collision.layer == CollisionLayer :: PLAYER2 || e1_collision.layer == CollisionLayer :: PLAYER1)){
                    if (collideWithCastle(entity1, entity2)){
                        ecsManager.publish(new CollisionEvent (entity1, entity2));
                    }
                }
                if (distance(e1_transform.position, e2_transform.position) < fmin(e1_collision.radius, e2_collision.radius) & e2_collision.layer != CollisionLayer :: Castle){
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

            if (e2_layer == CollisionLayer::Castle){
                ecsManager.publish(new WinEvent(e1));
            } else if (e1_layer == CollisionLayer::PLAYER1 & e2_layer == CollisionLayer::PLAYER2){
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

bool CollisionSystem::collideWithCastle(Entity player, Entity castle){
    auto &p_position = ecsManager.getComponent<Transform>(player).position;
    auto &p_boundingBox = ecsManager.getComponent<C_Collision>(player).boundingBox;

    auto &c_position = ecsManager.getComponent<Transform>(castle).position;
    auto &c_boundingBox = ecsManager.getComponent<C_Collision>(castle).boundingBox;


    float pl = p_position.x;
    float pr = pl + p_boundingBox.x;
    float pt = p_position.y;
    float pb = pt + p_boundingBox.y;

    float tt = c_position.y - c_boundingBox.y;
    float tb = tt + 2 * c_boundingBox.y;
    float tl = c_position.x- c_boundingBox.x;
    float tr = tl + 2 * c_boundingBox.x;
    CollisionResponse col_res = {false, false, false, false};

    col_res.left = (pr >= tl && pr <= tr); //approach from left
    col_res.right = (pl <= tr && pl >= tl); //approach from right
    bool x_over = (pr >= tl && pl <= tl); //overlap

    col_res.down = (pt >= tt && pt <= tb); // approach from bottom
    col_res.up = (pb <= tb && pb >= tt); //approach from top
    bool y_over =  (pb >= tb && pt <= tt); //overlap

    bool x_overlap = col_res.left || col_res.right || x_over;
    bool y_overlap = col_res.down || col_res.up || y_over;

    return x_overlap && y_overlap;
}