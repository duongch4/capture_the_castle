//
// Created by Tianyan Zhu on 2019-10-17.
//

#include "collision_system.hpp"
#include "math.h"
#include <ecs/ecs_manager.hpp>
#include <iostream>


extern ECSManager ecsManager;

void CollisionSystem::init()
{
	ecsManager.subscribe(this, &CollisionSystem::collisionListener);
	player_respawn_sound = Mix_LoadWAV(audio_path("capturethecastle_player_respawn.wav"));
	shield_pop_sound = Mix_LoadWAV(audio_path("capturethecastle_shield_pop.wav"));
	item_pick_up_sound = Mix_LoadWAV(audio_path("capturethecastle_item_pickup.wav"));
	bomb_explosion_sound = Mix_LoadWAV(audio_path("capturethecastle_bomb_explosion.wav"));
	flagMode = false;
}

void CollisionSystem::checkCollision()
{
	for (auto const& entity1 : entities)
	{
		for (auto const& entity2 : entities)
		{
			auto& e1_collision = ecsManager.getComponent<C_Collision>(entity1);
			auto& e2_collision = ecsManager.getComponent<C_Collision>(entity2);

			auto& e1_transform = ecsManager.getComponent<Transform>(entity1);
			auto& e2_transform = ecsManager.getComponent<Transform>(entity2);
			if (e1_collision.layer < e2_collision.layer)
			{
				if (e2_collision.layer == CollisionLayer::Castle && (e1_collision.layer == CollisionLayer::PLAYER2 || e1_collision.layer == CollisionLayer::PLAYER1))
				{
					if (collideWithCastle(entity1, entity2))
					{
						ecsManager.publish(new CollisionEvent(entity1, entity2));
					}
				}
				if (distance(e1_transform.position, e2_transform.position) <
					fmin(e1_collision.radius, e2_collision.radius) && e2_collision.layer != CollisionLayer::Castle)
				{
					ecsManager.publish(new CollisionEvent(entity1, entity2));
				}
			}
		}
	}
}

void CollisionSystem::update()
{
	entities_to_be_destroyed.clear();
	while (collision_queue.size() > 0)
	{
		std::pair<Entity, Entity> collision_pair = collision_queue.front();
		Entity e1 = collision_pair.first;
		Entity e2 = collision_pair.second;

		TeamType e1_team = ecsManager.getComponent<Team>(e1).assigned;
		TeamType e2_team = ecsManager.getComponent<Team>(e2).assigned;

		CollisionLayer e1_layer = ecsManager.getComponent<C_Collision>(e1).layer;
		CollisionLayer e2_layer = ecsManager.getComponent<C_Collision>(e2).layer;

		if ((e1_team == e2_team) && (!flagMode || (e2 != castle1 && e2 != castle2)))
		{
			collision_queue.pop();
			break;
		}
		else if (flagMode && e1_team == e2_team && (e2 == castle1 || e2 == castle2) && e1 == playerWithFlag)
		{
			ecsManager.publish(new WinEvent(e1));
			break;
		}
		else
		{
			auto& e1_transform = ecsManager.getComponent<Transform>(e1);
			auto& e2_transform = ecsManager.getComponent<Transform>(e2);
			MazeRegion region = Tilemap::get_region(e1_transform.position.x, e1_transform.position.y);

			if (e2 == castle1 || e2 == castle2)
			{
				// Handle castle collision
				if (!flagMode)
				{
					ecsManager.publish(new FlagEvent(e1, true));
					collision_queue.pop();
					break;
				}
			}
			else if (e1_layer == CollisionLayer::PLAYER1 && e2_layer == CollisionLayer::PLAYER2)
			{
				handle_player_player_collision(region, e1, e2, e1_transform, e2_transform);
			}
			else if (e2_layer == CollisionLayer::Enemy)
			{
				handle_player_enemy_collision(region, e1, e1_team, e1_transform, e2);
			}
			else if (e2_layer == CollisionLayer::Item)
			{
				handle_item_collision(e2, e1, e1_layer, e1_transform);
			}
			collision_queue.pop();
		}
	}


	std::set<Entity>::iterator it = entities_to_be_destroyed.begin();

	while (it != entities_to_be_destroyed.end())
	{
		ecsManager.destroyEntity(*it);
		it++;
	}
	entities_to_be_destroyed.clear();


}

void CollisionSystem::handle_item_collision(Entity& item, Entity& e1, CollisionLayer e1_layer, Transform& e1_transform)
{
	/// handle item collision
	auto& item_comp = ecsManager.getComponent<ItemComponent>(item);
	/// Bomb in_use, respawn player, delete enemy, delete itself
	if (item_comp.in_use && item_comp.itemType == ItemType::BOMB)
	{
		handle_bomb_in_use_collision(e1_layer, e1, item, e1_transform);
	}
	else
	{
		handle_item_not_bomb_in_use_collision(e1, e1_layer, item, item_comp);
	}
}

void CollisionSystem::handle_item_not_bomb_in_use_collision(Entity& player, CollisionLayer player_layer, Entity& item, ItemComponent& item_comp)
{
	if (player_layer != CollisionLayer::Enemy)
	{
		handle_item_pickup_removal(player, item, item_comp);
	}
}

void CollisionSystem::handle_item_pickup_removal(Entity& player, Entity& item, ItemComponent& item_comp)
{
	//if p1 or p2
	auto& player_item = ecsManager.getComponent<ItemComponent>(player);
	// Remove the current item from the player
	if (player_item.itemType == ItemType::SHIELD)
	{
		ecsManager.publish(new ItemEvent(player, ItemType::SHIELD, false));
	}
	else if (player_item.itemType == ItemType::BOMB)
	{
		ecsManager.publish(new ItemEvent(player, ItemType::BOMB, false));
	}
	// Pickup new item
	if (item_comp.itemType == ItemType::BOMB)
	{
		// pickup bomb
		player_item.itemType = ItemType::BOMB;
		ecsManager.publish(new ItemEvent(player, ItemType::BOMB, true));
	}
	else if (item_comp.itemType == ItemType::SHIELD)
	{
		// pick up shield and use shield
		player_item.itemType = ItemType::SHIELD;
		ecsManager.publish(new ItemEvent(player, ItemType::SHIELD, true));
	}
	Mix_PlayChannel(-1, item_pick_up_sound, 0);
	entities_to_be_destroyed.insert(item);
}

void CollisionSystem::handle_bomb_in_use_collision(CollisionLayer e1_layer, Entity& e1, Entity& item, Transform& e1_transform)
{
	if (e1_layer == CollisionLayer::Enemy)
	{
		handle_bomb_enemy_collision(e1, item);
	}
	else
	{
		handle_bomb_player_collision(e1, e1_transform, item);
	}
	Mix_PlayChannel(-1, bomb_explosion_sound, 0);
}

void CollisionSystem::handle_bomb_player_collision(Entity& e1, Transform& e1_transform, Entity& e2)
{
	// spawn player back to init location
	auto& player_item = ecsManager.getComponent<ItemComponent>(e1);
	if (player_item.itemType != ItemType::SHIELD)
	{
		handle_bomb_player_collision_no_shield(e1_transform, e1, e2);
	}
	else
	{
		handle_bomb_player_collision_with_shield(player_item, e1);
	}
	entities_to_be_destroyed.insert(e2);
}

void CollisionSystem::handle_bomb_player_collision_with_shield(ItemComponent& player_item, Entity& e1)
{
	player_item.itemType = ItemType::None;
	ecsManager.publish(new ItemEvent(e1, ItemType::SHIELD, false));
	Mix_PlayChannel(-1, shield_pop_sound, 0);
}

void CollisionSystem::handle_bomb_player_collision_no_shield(Transform& e1_transform, Entity& e1, Entity& e2)
{
	e1_transform.position = e1_transform.init_position;
	if (flagMode && e1 == playerWithFlag)
	{
		ecsManager.publish(new FlagEvent(e1, false));
		flagMode = false;
		playerWithFlag = 0;
		entities_to_be_destroyed.insert(bubble);
	}
	entities_to_be_destroyed.insert(e2);
	Mix_PlayChannel(-1, player_respawn_sound, 0);
}

void CollisionSystem::handle_bomb_enemy_collision(Entity& e1, Entity& e2)
{
	entities_to_be_destroyed.insert(e1);
	entities_to_be_destroyed.insert(e2);
}

void CollisionSystem::handle_player_enemy_collision(MazeRegion region, const Entity & player, TeamType player_team, Transform & player_transform, Entity & enemy)
{
	auto& player_item = ecsManager.getComponent<ItemComponent>(player);
	if (player_item.itemType != ItemType::SHIELD)
	{
		handle_player_enemy_collision_no_shield(region, player, player_team, player_transform);
	}
	else
	{
		handle_player_enemy_collision_with_shield(enemy, player, player_item);
	}
}

void CollisionSystem::handle_player_enemy_collision_with_shield(Entity & enemy, const Entity & player, ItemComponent& player_item)
{
	entities_to_be_destroyed.insert(enemy);
	ecsManager.publish(new ItemEvent(player, ItemType::SHIELD, false));
	player_item.itemType = ItemType::None;
	Mix_PlayChannel(-1, shield_pop_sound, 0);
}

void CollisionSystem::handle_player_enemy_collision_no_shield(MazeRegion region, const Entity & player, TeamType player_team, Transform & player_transform)
{
	switch (region)
	{
	case MazeRegion::PLAYER1:
		if (player_team == TeamType::PLAYER2)
		{
			player_transform.position = player_transform.init_position;
			if (flagMode && player == playerWithFlag)
			{
				ecsManager.publish(new FlagEvent(player, false));
				flagMode = false;
				playerWithFlag = 0;
				entities_to_be_destroyed.insert(bubble);
			}
			Mix_PlayChannel(-1, player_respawn_sound, 0);
		}
		break;
	case MazeRegion::PLAYER2:
		if (player_team == TeamType::PLAYER1)
		{
			player_transform.position = player_transform.init_position;
			if (flagMode && player == playerWithFlag)
			{
				ecsManager.publish(new FlagEvent(player, false));
				flagMode = false;
				playerWithFlag = 0;
				entities_to_be_destroyed.insert(bubble);
			}
			Mix_PlayChannel(-1, player_respawn_sound, 0);
		}
		break;
	case MazeRegion::BANDIT:
		player_transform.position = player_transform.init_position;
		if (flagMode && player == playerWithFlag)
		{
			ecsManager.publish(new FlagEvent(player, false));
			flagMode = false;
			playerWithFlag = 0;
			entities_to_be_destroyed.insert(bubble);
		}
		Mix_PlayChannel(-1, player_respawn_sound, 0);
		break;
	}
}

void CollisionSystem::handle_player_player_collision(MazeRegion region, Entity & player1, Entity & player2, Transform & player1_transform, Transform & player2_transform)
{
	///player vs player event
	if (!flagMode)
	{
		handle_player_player_collision_no_flag(region, player1, player2, player1_transform, player2_transform);
	}
	else
	{
		handle_player_player_collision_with_flag(player1, player2, player1_transform, player2_transform);
	}
}

void CollisionSystem::handle_player_player_collision_with_flag(const Entity & player1, const Entity & player2, Transform & player1_transform, Transform & player2_transform)
{
	if (player1 == playerWithFlag)
	{
		auto& player1_item = ecsManager.getComponent<ItemComponent>(player1);
		if (player1_item.itemType == ItemType::SHIELD)
		{
			ecsManager.getComponent<C_Collision>(player1).shield_effect_count = SHIELD_EFFECT_COUNT;
			ecsManager.publish(new ItemEvent(player1, ItemType::SHIELD, false));
			player1_item.itemType = ItemType::None;
			Mix_PlayChannel(-1, shield_pop_sound, 0);
		}
		else
		{
			int& wait = ecsManager.getComponent<C_Collision>(player1).shield_effect_count;
			if (wait > 0)
			{
				wait--;
				return;
			}
			player1_transform.position = player1_transform.init_position;
			Mix_PlayChannel(-1, player_respawn_sound, 0);

			ecsManager.publish(new FlagEvent(player1, false));
			flagMode = false;
			playerWithFlag = 0;
			entities_to_be_destroyed.insert(bubble);
		}
	}
	else if (player2 == playerWithFlag)
	{
		auto& player2_item = ecsManager.getComponent<ItemComponent>(player2);
		if (player2_item.itemType == ItemType::SHIELD)
		{
			ecsManager.getComponent<C_Collision>(player2).shield_effect_count = SHIELD_EFFECT_COUNT;
			ecsManager.publish(new ItemEvent(player2, ItemType::SHIELD, false));
			player2_item.itemType = ItemType::None;
			Mix_PlayChannel(-1, shield_pop_sound, 0);
		}
		else
		{
			int& wait = ecsManager.getComponent<C_Collision>(player2).shield_effect_count;
			if (wait > 0)
			{
				wait--;
				return;
			}
			player2_transform.position = player2_transform.init_position;
			Mix_PlayChannel(-1, player_respawn_sound, 0);

			ecsManager.publish(new FlagEvent(player2, false));
			flagMode = false;
			playerWithFlag = 0;
			entities_to_be_destroyed.insert(bubble);
		}
	}
}

void CollisionSystem::handle_player_player_collision_no_flag(MazeRegion region, const Entity & player1, const Entity & player2, Transform & player1_transform, Transform & player2_transform)
{
	auto& player2_item = ecsManager.getComponent<ItemComponent>(player2);
	auto& player1_item = ecsManager.getComponent<ItemComponent>(player1);

	switch (region)
	{
	case MazeRegion::PLAYER1:
		if (player2_item.itemType == ItemType::SHIELD)
		{
			ecsManager.getComponent<C_Collision>(player2).shield_effect_count = SHIELD_EFFECT_COUNT;
			ecsManager.publish(new ItemEvent(player2, ItemType::SHIELD, false));
			player2_item.itemType = ItemType::None;
			Mix_PlayChannel(-1, shield_pop_sound, 0);
		}
		else
		{
			int& wait = ecsManager.getComponent<C_Collision>(player2).shield_effect_count;
			if (wait > 0)
			{
				wait--;
				break;
			}
			player2_transform.position = player2_transform.init_position;
			Mix_PlayChannel(-1, player_respawn_sound, 0);
		}
		break;
	case MazeRegion::PLAYER2:
		if (player1_item.itemType == ItemType::SHIELD)
		{
			ecsManager.getComponent<C_Collision>(player1).shield_effect_count = SHIELD_EFFECT_COUNT;
			ecsManager.publish(new ItemEvent(player1, ItemType::SHIELD, false));
			player1_item.itemType = ItemType::None;
			Mix_PlayChannel(-1, shield_pop_sound, 0);
		}
		else
		{
			int& wait = ecsManager.getComponent<C_Collision>(player1).shield_effect_count;
			if (wait > 0)
			{
				wait--;
				break;
			}
			player1_transform.position = player1_transform.init_position;
			Mix_PlayChannel(-1, player_respawn_sound, 0);
		}
		break;
	case MazeRegion::BANDIT:
		break;
	}
}

float CollisionSystem::distance(vec2 e1, vec2 e2)
{
	float dx = e1.x - e2.x;
	float dy = e1.y - e2.y;
	return sqrt(dx * dx + dy * dy);
}

void CollisionSystem::collisionListener(CollisionEvent* collisionEvent)
{
	collision_queue.push(std::make_pair(collisionEvent->e1, collisionEvent->e2));
	std::pair<Entity, Entity> temp = collision_queue.front();
}

bool CollisionSystem::collideWithCastle(Entity player, Entity castle)
{
	auto& p_position = ecsManager.getComponent<Transform>(player).position;
	auto& p_boundingBox = ecsManager.getComponent<C_Collision>(player).boundingBox;

	auto& c_position = ecsManager.getComponent<Transform>(castle).position;
	auto& c_boundingBox = ecsManager.getComponent<C_Collision>(castle).boundingBox;
	auto& c_team = ecsManager.getComponent<Team>(castle).assigned;
	auto& p_team = ecsManager.getComponent<Team>(player).assigned;
	if (p_team == c_team && !flagMode)
		return false;

	float pl = p_position.x;
	float pr = pl + p_boundingBox.x;
	float pt = p_position.y;
	float pb = pt + p_boundingBox.y;

	float ct = c_position.y + 100;
	float cb = ct + c_boundingBox.y - 100;
	float cl = c_position.x - 50;
	float cr = cl + c_boundingBox.x + 50;
	CollisionResponse col_res = { false, false, false, false };

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

void CollisionSystem::reset()
{
	//tileMap->destroy();
	while (!collision_queue.empty())
	{
		collision_queue.pop();
	}
	this->entities.clear();
	if (player_respawn_sound != nullptr)
	{
		Mix_FreeChunk(player_respawn_sound);
	}
	if (shield_pop_sound != nullptr)
	{
		Mix_FreeChunk(shield_pop_sound);
	}
	if (item_pick_up_sound != nullptr)
	{
		Mix_FreeChunk(item_pick_up_sound);
	}
	if (bomb_explosion_sound != nullptr)
	{
		Mix_FreeChunk(bomb_explosion_sound);
	}
	//tileMap->destroy();
}

void CollisionSystem::setFlagMode(Entity flagPlayer)
{
	flagMode = true;
	playerWithFlag = flagPlayer;
}

void CollisionSystem::setBubble(Entity bubb)
{
	bubble = bubb;
}

void CollisionSystem::setCastle(Entity c1, Entity c2)
{
	castle1 = c1;
	castle2 = c2;
}
