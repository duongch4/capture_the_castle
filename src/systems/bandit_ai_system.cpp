//
// Created by Owner on 2019-10-16.
//

#include "bandit_ai_system.hpp"

#include <cmath>


bool BanditAISystem::init(std::shared_ptr<Tilemap> tm, Entity player_1, Entity player_2) {
	m_currentState = State::IDLE;
	m_target = player_1;
	m_target_2 = player_2;

	// Seeding rng with random device
	next_bandit_spawn = .0f;
	rng = std::default_random_engine(std::random_device()());
	tilemap = tm;
	return true;
}

void BanditAISystem::update(float elapsed_ms) {
	next_bandit_spawn -= elapsed_ms;
	if (entities.size() < MAX_BANDITS && next_bandit_spawn < 0.f)
	{
		m_bandit = spawn_bandit();

		// Next spawn
		next_bandit_spawn = (BANDIT_DELAY_MS / 2) + dist(rng) * (BANDIT_DELAY_MS / 2);
	}

	switch (m_currentState) {
	case State::IDLE:
		//if (next_bandit_spawn < 2.0f) {
		//	m_currentState = CHASE;
		//}
		ecsManager.getComponent<Motion>(m_bandit).direction = {0,0};
		break;
	case State::CHASE:
		//std::cout << (getDistance(m_target, m_bandit) > CHASE_THRESHOLD) << std::endl;
		if (getDistance(m_target, m_bandit) > CHASE_THRESHOLD) {
			m_currentState = State::IDLE;
			return;
		}
		followDirection(m_target, m_bandit, elapsed_ms);
		break;
	}
	checkTarget();
}

Entity BanditAISystem::spawn_bandit() {
	vec2 nextPos = tilemap->get_random_free_tile_position(MazeRegion::BANDIT);

	Entity bandit = ecsManager.createEntity();
	ecsManager.addComponent<BanditAIComponent>(bandit, BanditAIComponent{});
	ecsManager.addComponent<Transform>(bandit, Transform{
			nextPos,
			SCALE
		});
	ecsManager.addComponent<Motion>(bandit, Motion{
			INIT_DIRECTION,
			SPEED
		});
	ecsManager.addComponent<Team>(bandit, Team{ TeamType::BANDIT });
	Effect banditEffect{};
	banditEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(bandit, banditEffect);
	Sprite banditSprite = { textures_path("bandit/CaptureTheCastle_bandit_left.png") };
	TextureManager::instance()->load_from_file(banditSprite);
	ecsManager.addComponent<Sprite>(bandit, banditSprite);
	Mesh banditMesh{};
	banditMesh.init(banditSprite.width, banditSprite.height);
	ecsManager.addComponent<Mesh>(bandit, banditMesh);

	return bandit;
}

void BanditAISystem::setTarget(Entity target) {
	m_target = target;
}

void BanditAISystem::checkTarget() {
	if (m_currentState == State::CHASE) {
		return;
	}

	if (getDistance(m_target, m_bandit) < CHASE_THRESHOLD) {
		m_currentState = State::CHASE;
	}
}

float BanditAISystem::getDistance(Entity target, Entity bandit) {
	vec2 target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2 bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2 difference = { target_transform_pos.x - bandit_transform_pos.x, target_transform_pos.y - bandit_transform_pos.y };
	return (difference.x * difference.x) + (difference.y * difference.y);
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

	float distance = getDistance(target, bandit);
	float dir_x = target_transform_pos.x - bandit_transform_pos.x;
	float dir_y = target_transform_pos.y - bandit_transform_pos.y;
	bandit_motion_dir = { dir_x / sqrt(distance + 1e-5f), dir_y / sqrt(distance + 1e-5f) };
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
