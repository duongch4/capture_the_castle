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
        auto &boundingBox = ecsManager.getComponent<C_Collision>(e).boundingBox;

        float x_diff = abs(tile.get_position().x - transform.position.x);
        float y_diff = abs(tile.get_position().y - transform.position.y);

        // Compare the x and y distance between entity and tile to determine the direction to move the entity
        if (y_diff >= x_diff) {
            if (tile.get_position().y > transform.position.y) {
                transform.position.y = tile.get_position().y - tile.get_bounding_box().y/2 - boundingBox.y/2;
            } else {
                transform.position.y = tile.get_position().y + tile.get_bounding_box().y/2 + boundingBox.y/2;
            }
        } else if (x_diff > y_diff) {
            if (tile.get_position().x > transform.position.x) {
                transform.position.x = tile.get_position().x - tile.get_bounding_box().x/2 - boundingBox.x/2;
            } else {
                transform.position.x = tile.get_position().x + tile.get_bounding_box().x/2 + boundingBox.x/2;
            }
        }

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
                std::pair collisionPair = collides_with_tile(entity, tile);
                if (collisionPair.first){
                    ecsManager.publish(new BoxCollisionEvent(entity, tile, collisionPair.second));
                }

            }
        }
    }


}
std::pair<bool, CollisionResponse> BoxCollisionSystem::collides_with_tile(Entity entity, Tile &tile) {
    auto &position = ecsManager.getComponent<Transform>(entity).position;
    auto &boundingBox = ecsManager.getComponent<C_Collision>(entity).boundingBox;

    float pl = position.x - (boundingBox.x / 2);
    float pr = pl + boundingBox.x;
    float pt = position.y - (boundingBox.y / 2);
    float pb = pt + boundingBox.y;

    vec2  tile_pos = tile.get_position();
    vec2  tile_box = tile.get_bounding_box();
    float tt = tile_pos.y - (tile_box.y / 2);
    float tb = tt + tile_box.y;
    float tl = tile_pos.x - (tile_box.x / 2);
    float tr = tl + tile_box.x;
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

    float pt = position.y - (boundingBox.y / 2);
    float pb = pt + boundingBox.y;
    float tt = tile.get_position().y - (tile.get_bounding_box().y / 2);
    float tb = tt + tile.get_bounding_box().y;

    col_res.down = (pt >= tt && pt <= tb); // approach from bottom
    col_res.up = (pb <= tb && pb >= tt); //approach from top
    return col_res;

}

CollisionResponse BoxCollisionSystem::h_collision(Entity entity, Tile &tile, CollisionResponse col_res) {
    auto &position = ecsManager.getComponent<Transform>(entity).position;
    auto &boundingBox = ecsManager.getComponent<C_Collision>(entity).boundingBox;

    float pl = position.x - (boundingBox.x / 2);
    float pr = pl + boundingBox.x;
    float tl = tile.get_position().x - (tile.get_bounding_box().x / 2);
    float tr = tl + tile.get_bounding_box().x;

    col_res.left = (pr >= tl && pr <= tr); //approach from left
    col_res.right = (pl <= tr && pl >= tl); //approach from right
    return col_res;

}

void BoxCollisionSystem::boxCollisionListener(BoxCollisionEvent* boxCollisionEvent){
    collision_queue.push(std::tuple(boxCollisionEvent->e, boxCollisionEvent->tile, boxCollisionEvent->collisionResponse));
}

void BoxCollisionSystem::reset() {
	tileMap->destroy();
	while (!collision_queue.empty())
	{
		collision_queue.pop();
	}
	collision_queue = {};
	this->entities.clear();
}
