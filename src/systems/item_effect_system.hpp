//
// Created by Owner on 2019-11-21.
//

#ifndef CAPTURE_THE_CASTLE_ITEM_EFFECT_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_ITEM_EFFECT_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <ecs/events.hpp>
#include <texture_manager.hpp>

extern ECSManager ecsManager;

class ItemEffectSystem : public System {
public:
    void init() {
        mapPlayerToItem.clear();
        ecsManager.subscribe(this, &ItemEffectSystem::itemEventListener);
    };

    void update() {
        // for each item effect, update its position to follow its corresponding player's position
        for (auto playerEffectPair : mapPlayerToItem) {
            Entity player = playerEffectPair.first;
            Entity force_field = playerEffectPair.second;
            vec2 player_position = ecsManager.getComponent<Transform>(player).position;
            auto& force_field_transform = ecsManager.getComponent<Transform>(force_field);
            force_field_transform.old_position = force_field_transform.position;
            force_field_transform.position = player_position;
        }
    }

    void reset() override {
        mapPlayerToItem.clear();
    }

private:
    void itemEventListener(ItemEvent* itemEvent) {
        // currently the only item effect is the force shield
        if (itemEvent->item == ItemType::SHIELD) {
            // if the shield was just picked up, we want to create a force shield around this player
            if (itemEvent->pick_up) {
                Entity force_field = ecsManager.createEntity();

                Sprite force_field_sprite = {power_up_path("CaptureTheCastle_force_field.png")};
                TextureManager::instance().load_from_file(force_field_sprite, false);
                ecsManager.addComponent<Sprite>(force_field, force_field_sprite);

                Effect force_field_effect{};
                force_field_effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
                ecsManager.addComponent<Effect>(force_field, force_field_effect);

                MeshComponent force_field_mesh{MeshManager::instance().init_mesh(
                        force_field_sprite.width, force_field_sprite.height)};
                ecsManager.addComponent<MeshComponent>(force_field, force_field_mesh);

                Transform player_transform = ecsManager.getComponent<Transform>(itemEvent->player);
                Transform force_field_transform{
                        player_transform.position,
                        player_transform.position,
                        {0.7f, 0.7f},
                        player_transform.position};
                ecsManager.addComponent<Transform>(force_field, force_field_transform);
                // storing the player -> force shield effect in the map
                mapPlayerToItem.insert({itemEvent->player, force_field});
            } else {
                // if the shield was just dropped, we want to remove the force shield around the player
                if (mapPlayerToItem.find(itemEvent->player) != mapPlayerToItem.end()) {
                    Entity force_field_to_delete = mapPlayerToItem[itemEvent->player];
                    MeshComponent mesh_to_release = ecsManager.getComponent<MeshComponent>(force_field_to_delete);
                    MeshManager::instance().release(mesh_to_release.id);
                    ecsManager.destroyEntity(force_field_to_delete);
                    mapPlayerToItem.erase(itemEvent->player);
                }
            }
        }
    }

    std::map<Entity, Entity> mapPlayerToItem;
};

#endif //CAPTURE_THE_CASTLE_ITEM_EFFECT_SYSTEM_HPP
