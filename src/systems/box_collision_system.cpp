//
// Created by Tianyan Zhu on 2019-10-19.
//

#include <iostream>
#include "box_collision_system.hpp"

void BoxCollisionSystem::init(std::shared_ptr<Tilemap> tilemap) {
    this->tileMap = tilemap;
    ecsManager.subscribe(this, &BoxCollisionSystem::boxCollisionListener);
}

void BoxCollisionSystem::update() {
    while (collision_queue.size() > 0) {
        std::tuple<Entity, Tile, CollisionResponse> collision_tuple = collision_queue.front();
        Entity e = std::get<0>(collision_tuple);
        Tile tile = std::get<1>(collision_tuple);
        CollisionResponse col_res = std::get<2>(collision_tuple);

        auto &transform = ecsManager.getComponent<Transform>(e);
        auto &motion = ecsManager.getComponent<Motion>(e).direction;

        if (col_res.up || col_res.down) {
            motion.y = 0.f;
        } else if (col_res.right || col_res.left) {
            motion.x = 0.f;
        }
        auto &layer = ecsManager.getComponent<C_Collision>(e).layer;

        transform.position = transform.old_position;

        collision_queue.pop();
    }
}


void BoxCollisionSystem::checkCollision() {
    for(auto const& entity: entities) {
        auto &collision = ecsManager.getComponent<C_Collision>(entity);
        auto &transform = ecsManager.getComponent<Transform>(entity);
        std::vector<Tile> tiles = tileMap->get_adjacent_tiles(transform.position.x, transform.position.y);
        for (auto &tile : tiles) {
            if (tile.is_wall()) {
                CollisionResponse col_res = {false, false, false, false};
                std::pair collisionPair = collides_with_tile(entity, tile);
                if (collisionPair.first){
                    ecsManager.publish(new BoxCollisionEvent(entity, tile, col_res));
                }

            }
        }
    }


}
std::pair<bool, CollisionResponse> BoxCollisionSystem::collides_with_tile(Entity entity, Tile &tile) {
    auto &position = ecsManager.getComponent<Transform>(entity).position;
    auto &boundingBox = ecsManager.getComponent<C_Collision>(entity).boundingBox;

    float pl = position.x;
    float pr = pl + boundingBox.x;
    float pt = position.y;
    float pb = pt + boundingBox.y;

    float tt = tile.get_position().y;
    float tb = tt + tile.get_bounding_box().y;
    float tl = tile.get_position().x;
    float tr = tl + tile.get_bounding_box().x;
    CollisionResponse col_res = {false, false, false, false};

    col_res = v_collision(entity, tile, col_res);
    col_res = h_collision(entity, tile, col_res);
    bool x_over = (pr >= tl && pl <= tl); //overlap
    bool y_over = (pb >= tb && pt <= tt); //overlap

    bool x_overlap = col_res.left || col_res.right || x_over;
    bool y_overlap = col_res.down || col_res.up || y_over;

    if (x_overlap && y_overlap){
        return std::make_pair(true, col_res);
    } else {
        return std::make_pair(false, col_res);
    }
}

CollisionResponse BoxCollisionSystem::v_collision(Entity entity, Tile &tile, CollisionResponse col_res) {
    auto &position = ecsManager.getComponent<Transform>(entity).position;
    auto &boundingBox = ecsManager.getComponent<C_Collision>(entity).boundingBox;

    float pt = position.y;
    float pb = pt + boundingBox.y;
    float tt = tile.get_position().y;
    float tb = tt + tile.get_bounding_box().y;

    col_res.down = (pt >= tt && pt <= tb); // approach from bottom
    col_res.up = (pb <= tb && pb >= tt); //approach from top
    return col_res;

}

CollisionResponse BoxCollisionSystem::h_collision(Entity entity, Tile &tile, CollisionResponse col_res) {
    auto &position = ecsManager.getComponent<Transform>(entity).position;
    auto &boundingBox = ecsManager.getComponent<C_Collision>(entity).boundingBox;

    float pl = position.x;
    float pr = pl + boundingBox.x;
    float tl = tile.get_position().x;
    float tr = tl + tile.get_bounding_box().x;

    col_res.left = (pr >= tl && pr <= tr); //approach from left
    col_res.right = (pl <= tr && pl >= tl); //approach from right
    return col_res;

}

void BoxCollisionSystem::boxCollisionListener(BoxCollisionEvent* boxCollisionEvent){
    collision_queue.push(std::tuple(boxCollisionEvent->e, boxCollisionEvent->tile, boxCollisionEvent->collisionResponse));
}

void BoxCollisionSystem::reset() {

}
