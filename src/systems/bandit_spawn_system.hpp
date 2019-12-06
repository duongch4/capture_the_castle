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
	static const size_t get_max_bandits() { return MAX_BANDITS; }
	void reset() override;

private:
	const float SCALE_BOSS = 0.5f;
	const float SCALE_NORM = 0.09f;
	const vec2 SCALE_SHEET = { SCALE_NORM * 5.f / 7.f, SCALE_NORM };
	static const size_t MAX_BANDITS = 3;
    const float BANDIT_DELAY_MS = 10000;
    float next_bandit_spawn;
	const int boss_count = 0;
    std::shared_ptr<Tilemap> tilemap;

    // C++ rng
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist;

	void spawn_bandit(const BanditType& bandit_type, const vec2& transform_scale, const char* texture_path);
};


#endif //CAPTURE_THE_CASTLE_BANDIT_SPAWN_SYSTEM_HPP
