//
// Created by Tianyan Zhu on 2019-11-11.
//

#include <iostream>
#include "item_spawn_system.hpp"
#include "texture_manager.hpp"

extern ECSManager ecsManager;

bool ItemSpawnSystem::init(std::shared_ptr<Tilemap> tm) {
    next_item_spawn = 10.f;
    rng = std::default_random_engine(std::random_device()());
    tile_map = tm;
    return true;
}

void ItemSpawnSystem::update(float elapsed_ms) {
    next_item_spawn -= elapsed_ms;
    if (entities.size() < MAX_ITEMS && next_item_spawn < 0.f)
    {
        spawn_item();
        // Next spawn
        next_item_spawn = (ITEM_DELAY_MS / 2) + dist(rng) * (ITEM_DELAY_MS / 2);
    }
}


void ItemSpawnSystem::spawn_item(){

    int r = rand() % 3;
    vec2 nextPos;
    switch (r) {
        case 0:
            nextPos = tile_map->get_random_free_tile_position(MazeRegion::PLAYER1);
            break;
        case 1:
            nextPos = tile_map->get_random_free_tile_position(MazeRegion::PLAYER2);
            break;
        case 2:
            nextPos = tile_map->get_random_free_tile_position(MazeRegion::BANDIT);
            break;
    }

    Entity item = ecsManager.createEntity();
    int t = rand() % 2;
    ItemType item_type = ItemType::BOMB;
    if (t == 0){
        item_type = ItemType::SHIELD;
    }
    ecsManager.addComponent<ItemComponent>(item, ItemComponent{
            false,
            item_type
    });
    ecsManager.addComponent<Transform>(item, Transform{
            nextPos,
            nextPos,
            {0.8f, 0.8f},
            nextPos
    });

    Effect itemEffect{};
    itemEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
    ecsManager.addComponent<Effect>(item, itemEffect);
    Sprite itemSprite = {power_up_path("CaptureTheCastle_powerup_bomb.png")};
    if (t == 0)
        itemSprite = {power_up_path("CaptureTheCastle_powerup_shield.png")};
    TextureManager::instance()->load_from_file(itemSprite);
    itemSprite.sprite_size = { itemSprite.width / 7.0f , itemSprite.height / 5.0f };
    ecsManager.addComponent<Sprite>(item, itemSprite);
    MeshComponent itemMesh{MeshManager::instance()->init_mesh(
            itemSprite.width, itemSprite.height)};
    ecsManager.addComponent<MeshComponent>(item, itemMesh);
    float radius = itemSprite.width/2*0.08f;
    float i_width = itemSprite.width*0.08f*0.8f;
    float i_height = itemSprite.height*0.08f*0.8f;
    ecsManager.addComponent<C_Collision>(item, C_Collision{
            CollisionLayer::Item,
            radius,
            {i_width, i_height}
    });
}

void ItemSpawnSystem::reset() {

}

