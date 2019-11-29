//
// Created by Tianyan Zhu on 2019-11-11.
//

#ifndef CAPTURE_THE_CASTLE_ITEM_SPAWN_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_ITEM_SPAWN_SYSTEM_HPP

#include <memory>
#include <common.hpp>
#include <components.hpp>
#include <ecs/common_ecs.hpp>
#include <ecs/ecs_manager.hpp>
#include <tilemap.hpp>
#include <random>

extern ECSManager ecsManager;

class ItemSpawnSystem: public System {
public:
    bool init(std::shared_ptr<Tilemap> tile_map);
    void update(float ms);
    static const size_t get_max_items() { return MAX_ITEMS; }
    void reset() override ;


private:
    static const size_t MAX_ITEMS = 8;
    const size_t ITEM_DELAY_MS = 10000;
    float next_item_spawn;
    std::shared_ptr<Tilemap> tile_map;

    // C++ rng
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist;

    bool check_free_space(vec2 spawn_position);

    void spawn_item();
};


#endif //CAPTURE_THE_CASTLE_ITEM_SPAWN_SYSTEM_HPP
