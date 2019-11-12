//
// Created by Tianyan Zhu on 2019-10-19.
//

#ifndef CAPTURE_THE_CASTLE_BOX_COLLISION_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_BOX_COLLISION_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <components.hpp>
#include <ecs/events.hpp>
#include <tilemap.hpp>

extern ECSManager ecsManager;

class BoxCollisionSystem : public System {

public:
    void init(std::shared_ptr<Tilemap> tilemap);

    void checkCollision();

    void update();

    void reset() override;

private:
    std::shared_ptr<Tilemap> tileMap;
    std::pair<bool, CollisionResponse> collides_with_tile(Entity entity, Tile &tile);
    CollisionResponse h_collision(Entity entity, Tile &tile, CollisionResponse col_res);
    CollisionResponse v_collision(Entity entity, Tile &tile, CollisionResponse col_res);
    void boxCollisionListener(BoxCollisionEvent* boxCollisionEvent);
    std::queue<std::tuple<Entity, Tile, CollisionResponse>> collision_queue;
};

#endif //CAPTURE_THE_CASTLE_BOX_COLLISION_SYSTEM_HPP