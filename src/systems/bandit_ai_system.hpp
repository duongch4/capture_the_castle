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

class BanditAISystem: public System {
public:
	bool init(Entity player_1, Entity player_2);
    void update(float ms);

private:
	enum struct State {
		IDLE,
		CHASE,
	};

	const size_t CHASE_THRESHOLD = 200;
	const float SPEED = 70.f;

	State m_currentState;
	Entity m_bandit;
	std::vector<Entity> m_targets;
	void checkTarget(float distance_1, float distance_2);
	float getDistance(Entity target, Entity bandit);
	void followDirection(Entity target, Entity bandit, float elapsed_ms);
	bool isTargetMoveTowardBandit(vec2 bandit_transform_pos, vec2 target_transform_pos, vec2 target_motion_dir);
	bool isTargetMoveAwayBandit(vec2 bandit_transform_pos, vec2 target_transform_pos, vec2 target_motion_dir);
};


#endif //CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
