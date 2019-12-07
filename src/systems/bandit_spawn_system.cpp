//
// Created by Owner on 2019-10-16.
//

#include <mesh_manager.hpp>
#include "bandit_spawn_system.hpp"


bool BanditSpawnSystem::init(std::shared_ptr<Tilemap> tm)
{
	// Seeding rng with random device
	next_bandit_spawn = 0.f;
	rng = std::default_random_engine(std::random_device()());
	tilemap = tm;
	return true;
}

void BanditSpawnSystem::update(float elapsed_ms)
{
	next_bandit_spawn -= elapsed_ms;
	if (entities.size() < MAX_BANDITS && next_bandit_spawn < 0.f)
	{
		for (auto it = entities.begin(); it != entities.end(); ++it)
		{
			BanditType bandit_type = ecsManager.getComponent<BanditSpawnComponent>(*it).type;
			if (bandit_type == BanditType::BOSS)
			{
				spawn_bandit(BanditType::NORM, SCALE_SHEET, textures_path("bandit_sprite_sheet-01.png"));
				next_bandit_spawn = (BANDIT_DELAY_MS / 2.f) + dist(rng) * (BANDIT_DELAY_MS / 2.f);
				return;
			}
		}
		// No boss found
		spawn_bandit(BanditType::BOSS, { SCALE_BOSS, SCALE_BOSS }, textures_path("bandit_boss.png"));
		next_bandit_spawn = (BANDIT_DELAY_MS / 2.f) + dist(rng) * (BANDIT_DELAY_MS / 2.f);
	}
	else if (entities.size() == MAX_BANDITS)
	{
		next_bandit_spawn = BANDIT_DELAY_MS;
	}
}

void BanditSpawnSystem::spawn_bandit(const BanditType& bandit_type, const vec2& transform_scale, const char* texture_path)
{
	vec2 nextPos = tilemap->get_random_free_tile_position(MazeRegion::BANDIT);

	Entity bandit = ecsManager.createEntity();
	ecsManager.addComponent<BanditSpawnComponent>(bandit, BanditSpawnComponent{ bandit_type });
	ecsManager.addComponent<BanditAiComponent>(
		bandit,
		BanditAiComponent{
			BanditState::IDLE,
			0,0,0,
			vec2{ 0.f,0.f }
		}
	);
	ecsManager.addComponent<Transform>(
		bandit,
		Transform{
			nextPos,
			nextPos,
			transform_scale,
			nextPos
		}
	);
	ecsManager.addComponent<Motion>(
		bandit,
		Motion{
			{ 0.f, 0.f },
			100.f
		}
	);
	ecsManager.addComponent<Team>(bandit, Team{ TeamType::BANDIT });
	EffectComponent banditEffect{
	    shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")
	};
	EffectManager::instance().load_from_file(banditEffect);
	ecsManager.addComponent<EffectComponent>(bandit, banditEffect);
	Sprite banditSprite = { texture_path };
	TextureManager::instance().load_from_file(banditSprite);

	banditSprite.sprite_index = { 0, 0 };

	float collision_radius = 0.f, collision_width = 0.f, collision_height = 0.f;

	if (bandit_type == BanditType::NORM)
	{
		banditSprite.sprite_size = { banditSprite.width / 7.f , banditSprite.height / 5.f };

		collision_radius = banditSprite.width / 2.f * SCALE_NORM;
		collision_width = (float)banditSprite.width * SCALE_NORM * 0.8f;
		collision_height = (float)banditSprite.height * SCALE_NORM * 0.8f;
	}
	else if (bandit_type == BanditType::BOSS)
	{
		banditSprite.sprite_size = { (float)banditSprite.width, (float)banditSprite.height };

		collision_radius = banditSprite.width / 2.f;
		collision_width = (float)banditSprite.width * SCALE_BOSS;
		collision_height = (float)banditSprite.height * SCALE_BOSS;
	}

	ecsManager.addComponent<Sprite>(bandit, banditSprite);

	MeshComponent banditMesh{
		MeshManager::instance().init_mesh(
			banditSprite.width, banditSprite.height,
			banditSprite.sprite_size.x, banditSprite.sprite_size.y,
			banditSprite.sprite_index.x, banditSprite.sprite_index.y, 0
		)
	};
	ecsManager.addComponent<MeshComponent>(bandit, banditMesh);

	ecsManager.addComponent<C_Collision>(
		bandit,
		C_Collision{
			CollisionLayer::Enemy,
			collision_radius,
			{collision_width, collision_height}
		}
	);
}

void BanditSpawnSystem::reset()
{
	//tilemap->destroy();
	this->entities.clear();
}
