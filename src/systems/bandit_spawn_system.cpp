//
// Created by Owner on 2019-10-16.
//

#include <texture_manager.hpp>
#include "bandit_spawn_system.hpp"
#include "player_input_system.hpp"

bool BanditSpawnSystem::init() {
    // Seeding rng with random device
    next_bandit_spawn = .0f;
    rng = std::default_random_engine(std::random_device()());
    return true;
}

void BanditSpawnSystem::update(float elapsed_ms) {
    next_bandit_spawn -= elapsed_ms;
    if (entities.size() < MAX_BANDITS && next_bandit_spawn < 0.f)
    {
        spawn_bandit();

        // Next spawn
        next_bandit_spawn = (BANDIT_DELAY_MS / 2) + dist(rng) * (BANDIT_DELAY_MS / 2);
    }
}

void BanditSpawnSystem::spawn_bandit() {
    Entity bandit = ecsManager.createEntity();
    ecsManager.addComponent<BanditSpawnComponent>(bandit, BanditSpawnComponent{});
    ecsManager.addComponent<Transform>(bandit, Transform{
            { 299, 191},
            {0.4, 0.4}
    });
    ecsManager.addComponent<Motion>(bandit, Motion{
            {1, 0},
            100.f
    });
    ecsManager.addComponent<Team>(bandit, Team{TeamType::BANDIT});
    Effect banditEffect{};
    banditEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(bandit, banditEffect);
    Sprite banditSprite = {textures_path("bandit/CaptureTheCastle_bandit_right.png")};
    TextureManager::instance()->load_from_file(banditSprite);
    ecsManager.addComponent<Sprite>(bandit, banditSprite);
    Mesh banditMesh{};
    banditMesh.init(banditSprite.width, banditSprite.height);
    ecsManager.addComponent<Mesh>(bandit, banditMesh);
}