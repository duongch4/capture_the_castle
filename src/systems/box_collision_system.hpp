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

	bool should_collide(Tile& tile, TeamType& team_type, C_Collision& collision);

	bool is_soldier_in_bandit_region(TeamType& team_type, C_Collision& collision, Tile& tile);

	bool is_bandit_in_player_region(TeamType& team_type, Tile& tile);

    void update();

    void reset() override;

	void setFlagMode(bool mode, Entity flagPlayer, Entity bubb);

private:
    std::shared_ptr<Tilemap> tileMap;
    std::pair<bool, CollisionResponse> collides_with_tile(Entity entity, Tile &tile);
    CollisionResponse h_collision(Entity entity, Tile &tile, CollisionResponse col_res);
    CollisionResponse v_collision(Entity entity, Tile &tile, CollisionResponse col_res);
    void boxCollisionListener(BoxCollisionEvent* boxCollisionEvent);
    std::queue<std::tuple<Entity, Tile, CollisionResponse>> collision_queue;

	bool flag;
	Entity playerWithFlag;
	Entity bubble;
};

#endif //CAPTURE_THE_CASTLE_BOX_COLLISION_SYSTEM_HPP