//
// Created by Tianyan Zhu on 2019-11-13.
//

#ifndef CAPTURE_THE_CASTLE_ITEM_BOARD_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_ITEM_BOARD_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <ecs/events.hpp>
#include <ecs/common_ecs.hpp>


extern ECSManager ecsManager;

class ItemBoardSystem : public System{
public:
    void init();
    void update();
    void reset() override ;

private:
    void itemEventListener(ItemEvent* itemEvent);

};


#endif //CAPTURE_THE_CASTLE_ITEM_BOARD_SYSTEM_HPP
