//
// Created by Tianyan Zhu on 2019-11-13.
//

#include <texture_manager.hpp>
#include "item_board_system.hpp"

extern ECSManager ecsManager;

void ItemBoardSystem::init(){
    ecsManager.subscribe(this, &ItemBoardSystem::itemEventListener);
}

void ItemBoardSystem::update(){
    while (pick_up_queue.size() > 0) {
        std::tuple<Entity, bool, ItemType > item_tuple = pick_up_queue.front();
        Entity player = std::get<0>(item_tuple);
        bool pick_up = std::get<1>(item_tuple);
        ItemType itemType = std::get<2>(item_tuple);
        TeamType player_team = ecsManager.getComponent<Team>(player).assigned;
        for (auto& e: entities){
            if (ecsManager.getComponent<Team>(e).assigned == player_team){
                auto& item_texture = ecsManager.getComponent<Sprite>(e).texture_name;
                if (pick_up){
                    if (itemType == ItemType::BOMB){
                        item_texture = {power_up_path("CaptureTheCastle_powerup_bomb.png")};
                        //load bomb
                    } else {
                        item_texture = {power_up_path("CaptureTheCastle_powerup_shield.png")};
                        //load shield
                    }
                } else {
                    item_texture = {power_up_path("CaptureTheCastle_no_item.png")};
                }
            }
        }
        pick_up_queue.pop();
    }
}

void ItemBoardSystem::itemEventListener(ItemEvent *itemEvent) {
    pick_up_queue.push(std::tuple(itemEvent->player, itemEvent->pick_up, itemEvent->item));
}

void ItemBoardSystem::reset() {
	while (!pick_up_queue.empty())
	{
		pick_up_queue.pop();
	}
	pick_up_queue = {};
}