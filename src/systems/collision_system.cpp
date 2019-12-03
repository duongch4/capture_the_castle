//
// Created by Tianyan Zhu on 2019-10-17.
//

#include "collision_system.hpp"
#include "math.h"
#include <ecs/ecs_manager.hpp>
#include <iostream>


extern ECSManager ecsManager;

void CollisionSystem::init() {
    ecsManager.subscribe(this, &CollisionSystem::collisionListener);
    player_respawn_sound = Mix_LoadWAV(audio_path("capturethecastle_player_respawn.wav"));
}

void CollisionSystem::checkCollision() {
    for (auto const &entity1: entities) {
        for (auto const &entity2: entities) {
            auto &e1_collision = ecsManager.getComponent<C_Collision>(entity1);
            auto &e2_collision = ecsManager.getComponent<C_Collision>(entity2);

            auto& e1_transform = ecsManager.getComponent<Transform>(entity1);
            auto& e2_transform = ecsManager.getComponent<Transform>(entity2);
            if (e1_collision.layer < e2_collision.layer){
                if (e2_collision.layer == CollisionLayer::Castle && (e1_collision.layer == CollisionLayer::PLAYER2 || e1_collision.layer == CollisionLayer::PLAYER1)){
                    if (collideWithCastle(entity1, entity2)){
                        ecsManager.publish(new CollisionEvent (entity1, entity2));
                    }
                }
                if (distance(e1_transform.position, e2_transform.position) <
                    fmin(e1_collision.radius, e2_collision.radius) && e2_collision.layer != CollisionLayer::Castle) {
                    ecsManager.publish(new CollisionEvent(entity1, entity2));
                }
            }
        }
    }
}

void CollisionSystem::update() {
    entities_to_be_destroyed.clear();
    while (collision_queue.size() > 0) {
        std::pair<Entity, Entity> collision_pair = collision_queue.front();
        Entity e1 = collision_pair.first;
        Entity e2 = collision_pair.second;

        TeamType e1_team = ecsManager.getComponent<Team>(e1).assigned;
        TeamType e2_team = ecsManager.getComponent<Team>(e2).assigned;

        if (e1_team == e2_team) {
            collision_queue.pop();
            break;
        } else {
            CollisionLayer e1_layer = ecsManager.getComponent<C_Collision>(e1).layer;
            CollisionLayer e2_layer = ecsManager.getComponent<C_Collision>(e2).layer;

            auto &e1_transform = ecsManager.getComponent<Transform>(e1);
            auto &e2_transform = ecsManager.getComponent<Transform>(e2);
            MazeRegion region = Tilemap::get_region(e1_transform.position.x, e1_transform.position.y);

            if (e2_layer == CollisionLayer::Castle) {
                ///handle win event
                ecsManager.publish(new WinEvent(e1));
            } else if (e1_layer == CollisionLayer::PLAYER1 && e2_layer == CollisionLayer::PLAYER2) {
                ///player vs player event
                auto &player1_item = ecsManager.getComponent<ItemComponent>(e1);
                auto &player2_item = ecsManager.getComponent<ItemComponent>(e2);
                switch (region) {
                    case MazeRegion::PLAYER1:
                        if (player2_item.itemType != ItemType::SHIELD) {
                            e2_transform.position = e2_transform.init_position;
                            Mix_PlayChannel(-1, player_respawn_sound, 0);
                        } else {
                            ecsManager.publish(new ItemEvent(e2, ItemType::SHIELD, false));
                            player2_item.itemType = ItemType::None;
                        }
                        break;
                    case MazeRegion::PLAYER2:
                        if (player1_item.itemType != ItemType::SHIELD) {
                            e1_transform.position = e1_transform.init_position;
                            Mix_PlayChannel(-1, player_respawn_sound, 0);
                        } else {
                            ecsManager.publish(new ItemEvent(e1, ItemType::SHIELD, false));
                            player1_item.itemType = ItemType::None;
                        }
                        break;
                    case MazeRegion::BANDIT:
                        break;
                }
            } else if (e2_layer == CollisionLayer::Enemy) {
                /// handle player enemy collision
                auto &player_item = ecsManager.getComponent<ItemComponent>(e1);
                if (player_item.itemType != ItemType::SHIELD) {
                    switch (region) {
                        case MazeRegion::PLAYER1:
                            if (e1_team == TeamType::PLAYER2) {
                                e1_transform.position = e1_transform.init_position;
                                Mix_PlayChannel(-1, player_respawn_sound, 0);
                            }
                            break;
                        case MazeRegion::PLAYER2:
                            if (e1_team == TeamType::PLAYER1) {
                                e1_transform.position = e1_transform.init_position;
                                Mix_PlayChannel(-1, player_respawn_sound, 0);
                            }
                            break;
                        case MazeRegion::BANDIT:
                            e1_transform.position = e1_transform.init_position;
                            Mix_PlayChannel(-1, player_respawn_sound, 0);
                            break;
                    }
                } else {
                    entities_to_be_destroyed.insert(e2);
//                    ecsManager.destroyEntity(e2);
                    ecsManager.publish(new ItemEvent(e1, ItemType::SHIELD, false));
                    player_item.itemType = ItemType::None;
                }
            } else if (e2_layer == CollisionLayer::Item) {
                /// handle item collision
                auto &item = ecsManager.getComponent<ItemComponent>(e2);
                /// Bomb in_use, respawn player, delete enemy, delete itself
                if (item.in_use && item.itemType == ItemType::BOMB) {
                    if (e1_layer == CollisionLayer::Enemy) {
                        entities_to_be_destroyed.insert(e1);
                        entities_to_be_destroyed.insert(e2);

//                        ecsManager.destroyEntity(e1);
//                        ecsManager.destroyEntity(e2);
                    } else {
                        // spawn player back to init location
                        auto &player_item = ecsManager.getComponent<ItemComponent>(e1);
                        if (player_item.itemType != ItemType::SHIELD) {
                            e1_transform.position = e1_transform.init_position;
                            entities_to_be_destroyed.insert(e2);
//                            ecsManager.destroyEntity(e2);
                        } else {
                            player_item.itemType = ItemType::None;
                            ecsManager.publish(new ItemEvent(e1, ItemType::SHIELD, false));
                        }
                    }
                } else {
                    if (e1_layer != CollisionLayer::Enemy) {
                        //if p1 or p2
                        auto &player_item = ecsManager.getComponent<ItemComponent>(e1);
                        if (player_item.itemType == ItemType::None) {
                            if (item.itemType == ItemType::BOMB) {
                                // pickup bomb
                                player_item.itemType = ItemType::BOMB;
                                ecsManager.publish(new ItemEvent(e1, ItemType::BOMB, true));
                            } else {
                                // pick up shield and use shield
                                player_item.itemType = ItemType::SHIELD;
                                ecsManager.publish(new ItemEvent(e1, ItemType::SHIELD, true));
                            }
                            entities_to_be_destroyed.insert(e2);
//                            ecsManager.destroyEntity(e2);
                        }
                    }
                }
            }
            collision_queue.pop();
        }
    }



    std::set<Entity>::iterator it = entities_to_be_destroyed.begin();
    while (it != entities_to_be_destroyed.end()) {
//        std::cout<<*it<<std::endl;
        it++;
    }

    it = entities_to_be_destroyed.begin();

    while (it != entities_to_be_destroyed.end()) {
        ecsManager.destroyEntity(*it);
        it++;
//        entities_to_be_destroyed.erase(it);
    }
    entities_to_be_destroyed.clear();


}

float CollisionSystem::distance(vec2 e1, vec2 e2) {
    float dx = e1.x - e2.x;
    float dy = e1.y - e2.y;
    return sqrt(dx * dx + dy * dy);
}

void CollisionSystem::collisionListener(CollisionEvent *collisionEvent) {
    collision_queue.push(std::make_pair(collisionEvent->e1, collisionEvent->e2));
    std::pair<Entity, Entity> temp = collision_queue.front();
}

bool CollisionSystem::collideWithCastle(Entity player, Entity castle) {
    auto &p_position = ecsManager.getComponent<Transform>(player).position;
    auto &p_boundingBox = ecsManager.getComponent<C_Collision>(player).boundingBox;

    auto &c_position = ecsManager.getComponent<Transform>(castle).position;
    auto &c_boundingBox = ecsManager.getComponent<C_Collision>(castle).boundingBox;
    auto &c_team = ecsManager.getComponent<Team>(castle).assigned;
    auto &p_team = ecsManager.getComponent<Team>(player).assigned;
    if (p_team == c_team)
        return false;

    float pl = p_position.x;
    float pr = pl + p_boundingBox.x;
    float pt = p_position.y;
    float pb = pt + p_boundingBox.y;

    float ct = c_position.y + 100;
    float cb = ct + c_boundingBox.y - 100;
    float cl = c_position.x - 50 ;
    float cr = cl + c_boundingBox.x + 50;
    CollisionResponse col_res = {false, false, false, false};

    col_res.left = (pr >= cl && pr <= cr); //approach from left
    col_res.right = (pl <= cr && pl >= cl); //approach from right
    bool x_over = (pr >= cl && pl <= cl); //overlap

    col_res.down = (pt >= ct && pt <= cb); // approach from bottom
    col_res.up = (pb <= cb && pb >= ct); //approach from top
    bool y_over = (pb >= cb && pt <= ct); //overlap

    bool x_overlap = col_res.left || col_res.right || x_over;
    bool y_overlap = col_res.down || col_res.up || y_over;

    return x_overlap && y_overlap;
}

void CollisionSystem::reset() {
    //tileMap->destroy();
    while (!collision_queue.empty()) {
        collision_queue.pop();
    }
    this->entities.clear();
	//tileMap->destroy();
}
