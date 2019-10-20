//
// Created by Owner on 2019-10-16.
//

#include "bandit_ai_system.hpp"


bool BanditAISystem::init(Entity player_1, Entity player_2) {
	m_currentState = State::IDLE;
	m_targets.emplace_back(player_1);
	m_targets.emplace_back(player_2);
	return true;
}

void BanditAISystem::update(float elapsed_ms) {

	auto iter = entities.begin();
	m_bandit = 0;
	if (iter != entities.end()) m_bandit = *iter;

	float distance_1 = getDistance(m_targets[0], m_bandit);
	float distance_2 = getDistance(m_targets[1], m_bandit);

	switch (m_currentState) {
	case State::IDLE:
		//if (next_bandit_spawn < 2.0f) {
		//	m_currentState = CHASE;
		//}
		ecsManager.getComponent<Motion>(m_bandit).direction = {0,0};
		break;
	case State::CHASE:
		//std::cout << (getDistance(m_target, m_bandit) > CHASE_THRESHOLD) << std::endl;
		if (
			(distance_1 > CHASE_THRESHOLD) && (distance_2 > CHASE_THRESHOLD)
		) {
			m_currentState = State::IDLE;
			return;
		}
		if (distance_1 < distance_2) {
			followDirection(m_targets[0], m_bandit, elapsed_ms);
		}
		else {
			followDirection(m_targets[1], m_bandit, elapsed_ms);
		}
		break;
	}
	checkTarget(distance_1, distance_2);
}

void BanditAISystem::checkTarget(float distance_1, float distance_2) {
	if (m_currentState == State::CHASE) {
		return;
	}

	if ((distance_1 < CHASE_THRESHOLD) || (distance_2 < CHASE_THRESHOLD)) {
		m_currentState = State::CHASE;
	}
}

float BanditAISystem::getDistance(Entity target, Entity bandit) {
	vec2 target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2 bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2 difference = { target_transform_pos.x - bandit_transform_pos.x, target_transform_pos.y - bandit_transform_pos.y };
	return std::sqrtf((difference.x * difference.x) + (difference.y * difference.y));
}

void BanditAISystem::followDirection(Entity target, Entity bandit, float elapsed_ms) {
	vec2& target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2& bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& target_motion_dir = ecsManager.getComponent<Motion>(target).direction;
	vec2& bandit_motion_dir = ecsManager.getComponent<Motion>(bandit).direction;

	float step = SPEED * elapsed_ms / 1000.f;

	if (
		isTargetMoveTowardBandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
	) {
		bandit_transform_pos.x -= target_motion_dir.x * step;
		bandit_transform_pos.y -= target_motion_dir.y * step;
	}
	if (
		isTargetMoveAwayBandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
	) {
		bandit_transform_pos.x += target_motion_dir.x * step;
		bandit_transform_pos.y += target_motion_dir.y * step;
	}

	float distance = getDistance(target, bandit) + 1e-5f;
	float dir_x = target_transform_pos.x - bandit_transform_pos.x;
	float dir_y = target_transform_pos.y - bandit_transform_pos.y;
	bandit_motion_dir = { dir_x / distance, dir_y / distance };
	std::cout << "x" << std::endl;
	std::cout << bandit_motion_dir.x << std::endl;
	std::cout << "y" << std::endl;
	std::cout << bandit_motion_dir.y << std::endl;
}

bool BanditAISystem::isTargetMoveTowardBandit(
	vec2 bandit_transform_pos, vec2 target_transform_pos, vec2 target_motion_dir
) {
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y < 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y > 0.f))
	);
}

bool BanditAISystem::isTargetMoveAwayBandit(
	vec2 bandit_transform_pos, vec2 target_transform_pos, vec2 target_motion_dir
) {
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y > 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y < 0.f))
	);
}
