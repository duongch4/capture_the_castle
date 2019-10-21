//
// Created by Owner on 2019-10-16.
//

#ifndef CAPTURE_THE_CASTLE_BANDIT_SPAWN_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_BANDIT_SPAWN_SYSTEM_HPP

#include <memory>
#include <random>

#include <systems/player_input_system.hpp>
#include <ecs/common_ecs.hpp>
#include <tilemap.hpp>
#include <texture_manager.hpp>


class BanditSpawnSystem: public System {
public:
    bool init(std::shared_ptr<Tilemap> tilemap);
    void update(float ms);

private:
    const size_t MAX_BANDITS = 2;
    const size_t BANDIT_DELAY_MS = 2000;
    float next_bandit_spawn;
    std::shared_ptr<Tilemap> tilemap;

    // C++ rng
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist;

	void spawn_bandit();
};


#endif //CAPTURE_THE_CASTLE_BANDIT_SPAWN_SYSTEM_HPP
