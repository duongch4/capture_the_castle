//
// Created by Owner on 2019-10-16.
//

#ifndef CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP

#include <memory>
#include <random>

#include <iostream>

#include <ecs/common_ecs.hpp>
#include <ecs/ecs_manager.hpp>
#include <components.hpp>

extern ECSManager ecsManager;

class BanditAISystem : public System
{
public:
	bool init(Entity& player_1, Entity& player_2);
	void update(float& ms);

private:
	enum struct State
	{
		IDLE,
		CHASE,
	};

	const size_t MAX_BANDITS = 5;
	const size_t IDLE_LIMIT = 30; // beats
	const size_t CHASE_THRESHOLD = 200;
	const size_t CHASE_LIMIT = 100; // beats
	const float BASE_SPEED = 70.f;

	std::vector<size_t> m_idleTimes;
	std::vector<size_t> m_chaseTimes;
	std::vector<State> m_states;
	std::vector<Entity> m_bandits;
	std::vector<Entity> m_targets;

	float getDistance(Entity& target, Entity& bandit);

	void handleIdle(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);
	bool canChase(float& distance_1, float& distance_2, size_t& idle_time);

	void handleChase(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);
	bool cannotChase(float& distance_1, float& distance_2, size_t& chase_time);
	void chase(float& distance_1, float& distance_2, Entity& bandit, float& speed, float& elapsed_ms);
	void followDirection(Entity& target, Entity& bandit, float& speed, float& elapsed_ms);
	bool isTargetMoveTowardBandit(vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir);
	bool isTargetMoveAwayBandit(vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir);

	// C++ rng
	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist;
};


#endif //CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
