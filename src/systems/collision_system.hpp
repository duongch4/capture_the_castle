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
	int SHIELD_EFFECT_COUNT = 15;
private:
	void handle_item_collision(Entity& e2, CollisionLayer e1_layer, Entity& e1, Transform& e1_transform);
	void handle_shield_collision(CollisionLayer e1_layer, Entity& e1, ItemComponent& item, Entity& e2);
	void handle_shield_collision_with_other_player(Entity& e1, ItemComponent& item, Entity& e2);
	void handle_bomb_collision(CollisionLayer e1_layer, Entity& e1, Entity& e2, Transform& e1_transform);
	void handle_bomb_player_collision(Entity& e1, Transform& e1_transform, Entity& e2);
	void handle_bomb_player_collision_with_shield(ItemComponent& player_item, Entity& e1);
	void handle_bomb_player_collision_no_shield(Transform& e1_transform, Entity& e1, Entity& e2);
	void handle_bomb_enemy_collision(Entity& e1, Entity& e2);
	void handle_player_enemy_collision(MazeRegion region, const Entity & player, TeamType player_team, Transform & player_transform, Entity & enemy);
	void handle_player_enemy_collision_with_shield(Entity & enemy, const Entity & player, ItemComponent& player_item);
	void handle_player_enemy_collision_no_shield(MazeRegion region, const Entity & player, TeamType player_team, Transform & player_transform);
	void handle_player_player_collision(MazeRegion region, Entity & player1, Entity & player2, Transform & player1_transform, Transform & player2_transform);
	void handle_player_player_collision_with_flag(const Entity & player1, const Entity & player2, Transform & player1_transform, Transform & player2_transform);
	void handle_player_player_collision_no_flag(MazeRegion region, const Entity & player1, const Entity & player2, Transform & player1_transform, Transform & player2_transform);

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

#endif //CAPTURE_THE_CASTLE_COLLISION_SYSTEM_HPP