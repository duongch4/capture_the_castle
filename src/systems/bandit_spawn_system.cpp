//
// Created by Owner on 2019-10-16.
//

#include "bandit_spawn_system.hpp"


bool BanditSpawnSystem::init(std::shared_ptr<Tilemap> tm) {
    // Seeding rng with random device
    next_bandit_spawn = .0f;
    rng = std::default_random_engine(std::random_device()());
    tilemap = tm;
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
    vec2 nextPos = tilemap->get_random_free_tile_position(MazeRegion::BANDIT);

    Entity bandit = ecsManager.createEntity();
    ecsManager.addComponent<BanditSpawnComponent>(bandit, BanditSpawnComponent{});
    ecsManager.addComponent<Transform>(bandit, Transform{
            nextPos,
			{0.08f, 0.08f}
    });
    ecsManager.addComponent<Motion>(bandit, Motion{
            {1, 0},
            100.f
    });
    ecsManager.addComponent<Team>(bandit, Team{TeamType::BANDIT});
	ecsManager.addComponent<BanditAIComponent>(bandit, BanditAIComponent{});
    Effect banditEffect{};
    banditEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(bandit, banditEffect);
    Sprite banditSprite = {textures_path("bandit_sprite_sheet-01.png")};
    TextureManager::instance()->load_from_file(banditSprite);
	banditSprite.sprite_index = { 0 , 3 };
	banditSprite.sprite_size = { banditSprite.width / 7.0f , banditSprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(bandit, banditSprite);
    Mesh banditMesh{};
    banditMesh.init(
		banditSprite.width, banditSprite.height, (int) banditSprite.sprite_size.x, (int) banditSprite.sprite_size.y,
		(int) banditSprite.sprite_index.x, (int) banditSprite.sprite_index.y, 0
	);
    ecsManager.addComponent<Mesh>(bandit, banditMesh);
}