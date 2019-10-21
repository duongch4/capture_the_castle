#include "bandit_ai_system.hpp"

bool BanditAISystem::init(Entity& player_1, Entity& player_2)
{
	m_targets.emplace_back(player_1);
	m_targets.emplace_back(player_2);
	for (size_t i = 0; i < MAX_BANDITS; ++i)
	{
		m_idleTimes.emplace_back(0);
		m_chaseTimes.emplace_back(0);
		m_states.emplace_back(State::IDLE);
	}
	return true;
}

void BanditAISystem::update(float& elapsed_ms)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		auto idx = std::distance(entities.begin(), it);
		Entity bandit = *it;
		State& state = m_states[idx];
		size_t& idleTime = m_idleTimes[idx];
		size_t& chaseTime = m_chaseTimes[idx];
		float speed = BASE_SPEED * (1.f + dist(rng));
		//std::cout << speed << std::endl;

		float distance_1 = getDistance(m_targets[0], bandit);
		float distance_2 = getDistance(m_targets[1], bandit);

		switch (state)
		{
		case State::IDLE:
			handleIdle(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
			break;
		case State::CHASE:
			handleChase(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
			break;
		}
	}
}

void BanditAISystem::handleIdle(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& speed, float& elapsed_ms
)
{
	if (canChase(distance_1, distance_2, idle_time))
	{
		state = State::CHASE;
		chase_time = 0;
		return;
	}

	ecsManager.getComponent<Motion>(bandit).direction = { 0,0 };
	idle_time++;
}

bool BanditAISystem::canChase(float& distance_1, float& distance_2, size_t& idle_time)
{
	return (
		((distance_1 < CHASE_THRESHOLD) || (distance_2 < CHASE_THRESHOLD)) &&
		(idle_time > IDLE_LIMIT)
		);
}

void BanditAISystem::handleChase(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& speed, float& elapsed_ms
)
{
//	std::cout << chase_time << std::endl;
	if (cannotChase(distance_1, distance_2, chase_time))
	{
		state = State::IDLE;
		idle_time = 0;
		return;
	}

	chase(distance_1, distance_2, bandit, speed, elapsed_ms);
	chase_time++;
}

bool BanditAISystem::cannotChase(float& distance_1, float& distance_2, size_t& chase_time)
{
	return (
		((distance_1 > CHASE_THRESHOLD) && (distance_2 > CHASE_THRESHOLD)) ||
		(chase_time > CHASE_LIMIT)
		);
}

void BanditAISystem::chase(float& distance_1, float& distance_2, Entity& bandit, float& speed, float& elapsed_ms)
{
	if (distance_1 < distance_2)
	{
		followDirection(m_targets[0], bandit, speed, elapsed_ms);
	}
	else
	{
		followDirection(m_targets[1], bandit, speed, elapsed_ms);
	}
}

float BanditAISystem::getDistance(Entity& target, Entity& bandit)
{
	vec2 target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2 bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2 difference = { target_transform_pos.x - bandit_transform_pos.x, target_transform_pos.y - bandit_transform_pos.y };
	return sqrtf((difference.x * difference.x) + (difference.y * difference.y));
}

void BanditAISystem::followDirection(Entity& target, Entity& bandit, float& speed, float& elapsed_ms)
{
	vec2& target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2& bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& target_motion_dir = ecsManager.getComponent<Motion>(target).direction;
	vec2& bandit_motion_dir = ecsManager.getComponent<Motion>(bandit).direction;

	float step = speed * elapsed_ms / 1000.f;

	if (
		isTargetMoveTowardBandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		bandit_transform_pos.x -= target_motion_dir.x * step;
		bandit_transform_pos.y -= target_motion_dir.y * step;
	}
	if (
		isTargetMoveAwayBandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		bandit_transform_pos.x += target_motion_dir.x * step;
		bandit_transform_pos.y += target_motion_dir.y * step;
	}

	float distance = getDistance(target, bandit) + 1e-5f;
	float dir_x = target_transform_pos.x - bandit_transform_pos.x;
	float dir_y = target_transform_pos.y - bandit_transform_pos.y;
	bandit_motion_dir = { dir_x / distance, dir_y / distance };
	//std::cout << "x" << std::endl;
	//std::cout << bandit_motion_dir.x << std::endl;
	//std::cout << "y" << std::endl;
	//std::cout << bandit_motion_dir.y << std::endl;
}

bool BanditAISystem::isTargetMoveTowardBandit(
	vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir
)
{
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y < 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y > 0.f))
		);
}

bool BanditAISystem::isTargetMoveAwayBandit(
	vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir
)
{
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y > 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y < 0.f))
		);
}
