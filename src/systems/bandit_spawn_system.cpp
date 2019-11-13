//
// Created by Owner on 2019-10-16.
//

#include <mesh_manager.hpp>
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
            nextPos,
            {0.09f * 5 / 7, 0.09f},
            nextPos
    });
    ecsManager.addComponent<Motion>(bandit, Motion{
            {1, 0},
            100.f
    });
    ecsManager.addComponent<Team>(bandit, Team{TeamType::BANDIT});
	ecsManager.addComponent<BanditAiComponent>(bandit, BanditAiComponent{});
    Effect banditEffect{};
    banditEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(bandit, banditEffect);
    Sprite banditSprite = {textures_path("bandit_sprite_sheet-01.png")};
    TextureManager::instance()->load_from_file(banditSprite);
	banditSprite.sprite_index = { 0 , 3 };
	banditSprite.sprite_size = { banditSprite.width / 7.0f , banditSprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(bandit, banditSprite);
    MeshComponent banditMesh{MeshManager::instance().init_mesh(
            banditSprite.width, banditSprite.height, banditSprite.sprite_size.x, banditSprite.sprite_size.y,
            banditSprite.sprite_index.x, banditSprite.sprite_index.y, 0)
    };
    ecsManager.addComponent<MeshComponent>(bandit, banditMesh);
    float radius = banditSprite.width/2*0.08f;
    float b_width = banditSprite.width*0.08f*0.8f;
    float b_height = banditSprite.height*0.08f*0.8f;
    ecsManager.addComponent<C_Collision>(bandit, C_Collision{
            CollisionLayer::Enemy,
            radius,
            {b_width, b_height}
    });
}

void BanditSpawnSystem::reset() {
	tilemap->destroy();
	this->entities.clear();
}
