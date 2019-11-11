//#include "soldier_spawn_system.hpp"
//
//
//bool SoldierSpawnSystem::init(std::shared_ptr<Tilemap> tm)
//{
//	// Seeding rng with random device
//	next_soldier_spawn = .0f;
//	rng = std::default_random_engine(std::random_device()());
//	tilemap = tm;
//	return true;
//}
//
//void SoldierSpawnSystem::update(float elapsed_ms)
//{
//	next_soldier_spawn -= elapsed_ms;
//	if (entities.size() < MAX_SOLDIERS && next_soldier_spawn < 0.f)
//	{
//		spawn_soldier();
//
//		// Next spawn
//		next_soldier_spawn = (SOLDIER_DELAY_MS / 2) + dist(rng) * (SOLDIER_DELAY_MS / 2);
//	}
//}
//
//void SoldierSpawnSystem::spawn_soldier() {
//    vec2 nextPos = tilemap->get_random_free_tile_position(MazeRegion::PLAYER1);
//
//    Entity soldier = ecsManager.createEntity();
//    ecsManager.addComponent<BanditSpawnComponent>(soldier, BanditSpawnComponent{});
//    ecsManager.addComponent<Transform>(soldier, Transform{
//            nextPos,
//            nextPos,
//            {0.08f, 0.08f},
//            nextPos
//    });
//    ecsManager.addComponent<Motion>(soldier, Motion{
//            {1, 0},
//            100.f
//    });
//    ecsManager.addComponent<Team>(soldier, Team{TeamType::PLAYER1});
//	ecsManager.addComponent<BanditAIComponent>(soldier, BanditAIComponent{});
//    Effect soldierEffect{};
//    soldierEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
//    ecsManager.addComponent<Effect>(soldier, soldierEffect);
//    Sprite soldierSprite = {textures_path("soldier_sprite_sheet-01.png")};
//    TextureManager::instance()->load_from_file(soldierSprite);
//	soldierSprite.sprite_index = { 0 , 3 };
//	soldierSprite.sprite_size = { soldierSprite.width / 7.0f , soldierSprite.height / 5.0f };
//    ecsManager.addComponent<Sprite>(soldier, soldierSprite);
//    Mesh soldierMesh{};
//    soldierMesh.init(
//		soldierSprite.width, soldierSprite.height, soldierSprite.sprite_size.x, soldierSprite.sprite_size.y,
//		soldierSprite.sprite_index.x, soldierSprite.sprite_index.y, 0
//	);
//    ecsManager.addComponent<Mesh>(soldier, soldierMesh);
//    float radius = soldierSprite.width/2*0.08f;
//    float b_width = soldierSprite.width*0.08f*0.8f;
//    float b_height = soldierSprite.height*0.08f*0.8f;
//    ecsManager.addComponent<C_Collision>(soldier, C_Collision{
//            CollisionLayer::Enemy,
//            radius,
//            {b_width, b_height}
//    });
//}