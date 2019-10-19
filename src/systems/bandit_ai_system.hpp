//
// Created by Owner on 2019-10-16.
//

#ifndef CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP

#include <memory>
#include <random>


#include <iostream>


#include <ecs/common_ecs.hpp>
#include <systems/player_input_system.hpp>
#include <tilemap.hpp>
#include <texture_manager.hpp>


class BanditAISystem: public System {
public:
	bool init(std::shared_ptr<Tilemap> tm, Entity player_1, Entity player_2);
    void update(float ms);

private:
	enum State {
		IDLE,
		CHASE,
	};
	const size_t MAX_BANDITS = 1;
	const size_t BANDIT_DELAY_MS = 15000;
	const size_t CHASE_THRESHOLD = 10000;
	const vec2 SCALE = {0.4f, 0.4f};
	const vec2 INIT_DIRECTION = {0.f, 0.f};
	const float SPEED = 200.f;

	State m_currentState;
	Entity m_bandit;
	Entity m_target;
	Entity m_target_2;
	void setTarget(Entity target);
	void checkTarget();
	float getDistance(Entity target, Entity bandit);
	void followDirection(Entity target, Entity bandit);

    float next_bandit_spawn;
    std::shared_ptr<Tilemap> tilemap;

    // C++ rng
    std::default_random_engine rng;
    std::uniform_real_distribution<float> dist;

    Entity spawn_bandit();
};


#endif //CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
