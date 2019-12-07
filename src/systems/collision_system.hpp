//
// Created by Tianyan Zhu on 2019-10-17.
//

#ifndef CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP

#include <SDL_mixer.h>

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
	void setFlagMode(Entity flagPlayer);
	void setBubble(Entity bubb);
    void reset() override;
	void setCastle(Entity c1, Entity c2);
private:
	void handle_item_collision(const Entity& e2, const CollisionLayer& e1_layer, const Entity& e1, Transform& e1_transform);
	void handle_player_enemy_collision(const Entity& e1, const MazeRegion& region, const TeamType& e1_team, Transform& e1_transform, const Entity& e2);
	void handle_player_player_collision(const Entity& e1, const Entity& e2, const MazeRegion& region, Transform& e2_transform, Transform& e1_transform);

private:
    void collisionListener(CollisionEvent* collisionEvent);
    bool collideWithCastle(Entity player, Entity castle);
    float distance(vec2 e1, vec2 e2);
    std::queue<std::pair<Entity, Entity>> collision_queue;
    std::set<Entity> entities_to_be_destroyed;
	Entity playerWithFlag;
	Entity bubble;
	Entity castle1;
	Entity castle2;
	bool flagMode;
    Mix_Chunk* player_respawn_sound;
    Mix_Chunk* shield_pop_sound;
    Mix_Chunk* item_pick_up_sound;
    Mix_Chunk* bomb_explosion_sound;
};
//

#endif //CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP