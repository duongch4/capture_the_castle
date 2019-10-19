//
// Created by Owner on 2019-10-16.
//

#include "bandit_ai_system.hpp"


bool BanditAISystem::init(std::shared_ptr<Tilemap> tm, Entity player_1, Entity player_2) {
	m_currentState = IDLE;
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
	case IDLE:
		if (next_bandit_spawn < 2.0f) {
			m_currentState = CHASE;
		}
		break;
	case CHASE:
		Transform& transform = ecsManager.getComponent<Transform>(m_target);
		Transform& m_transform = ecsManager.getComponent<Transform>(m_bandit);
		vec2 difference = vec2{ transform.position.x - m_transform.position.x, transform.position.y - m_transform.position.y };
		float distance = (difference.x * difference.x) + (difference.y * difference.y);

		std::cout << distance << std::endl;
		//if (distance > CHASE_THRESHOLD) {
		//	m_currentState = IDLE;
		//	return;
		//}
		if (distance < CHASE_THRESHOLD) {
			Motion& m_motion = ecsManager.getComponent<Motion>(m_bandit);
			Motion& motion = ecsManager.getComponent<Motion>(m_target);
			m_motion.direction = motion.direction;
		}
	}
	checkTarget(m_bandit);
}

Entity BanditAISystem::spawn_bandit() {
	vec2 nextPos = tilemap->get_random_free_tile_position(MazeRegion::BANDIT);

	Entity bandit = ecsManager.createEntity();
	ecsManager.addComponent<BanditAIComponent>(bandit, BanditAIComponent{});
	ecsManager.addComponent<Transform>(bandit, Transform{
			nextPos,
			{0.4, 0.4}
		});
	ecsManager.addComponent<Motion>(bandit, Motion{
			{0, 0},
			150.f
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

void BanditAISystem::checkTarget(Entity bandit) {
	if (m_currentState == CHASE) {
		return;
	}

	Transform transform = ecsManager.getComponent<Transform>(m_target);
	Transform m_transform = ecsManager.getComponent<Transform>(bandit);
	vec2 difference = vec2{ transform.position.x - m_transform.position.x, transform.position.y - m_transform.position.y };
	float distance = (difference.x * difference.x) + (difference.y * difference.y);
	if (distance < CHASE_THRESHOLD) {
		m_currentState = CHASE;
	}
}