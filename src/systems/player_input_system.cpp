//
// Created by Owner on 2019-10-12.
//


#include "player_input_system.hpp"

extern ECSManager ecsManager;

void PlayerInputSystem::init(std::shared_ptr<Tilemap> tilemap)
{
	ecsManager.subscribe(this, &PlayerInputSystem::onKeyListener);
	ecsManager.subscribe(this, &PlayerInputSystem::onReleaseListener);
	m_tilemap = tilemap;
}

void PlayerInputSystem::update()
{
	for (auto& e : entities)
	{
		auto& motion = ecsManager.getComponent<Motion>(e);
		auto& transform = ecsManager.getComponent<Transform>(e);
		auto& team = ecsManager.getComponent<Team>(e);
		vec2 next_dir = { 0, 0 };
		if (team.assigned == TeamType::PLAYER1)
		{
			for (auto key : PLAYER1KEYS)
			{
				if (keysPressed[key])
				{
					Tile tile = m_tilemap->get_tile(transform.position.x, transform.position.y);
					switch (key)
					{
					case InputKeys::W:
						next_dir.y -= 1;
						break;
					case InputKeys::S:
						next_dir.y += 1;
						break;
					case InputKeys::D:
						next_dir.x += 1;
						break;
					case InputKeys::A:
						next_dir.x -= 1;
						break;
					case InputKeys::Q:
						handle_soldier_spawn(
							soldier_count_1, wait_1, transform, tile,
							MazeRegion::PLAYER1, TeamType::PLAYER1,
							textures_path("red_soldier_sprite_sheet-01.png")
						);
						break;
					default:
						break;
					}
				}
			}
			++wait_1;
		}
		else if (team.assigned == TeamType::PLAYER2)
		{
			for (auto key : PLAYER2KEYS)
			{
				if (keysPressed[key])
				{
					Tile tile = m_tilemap->get_tile(transform.position.x, transform.position.y);
					switch (key)
					{
					case InputKeys::UP:
						next_dir.y -= 1;
						break;
					case InputKeys::DOWN:
						next_dir.y += 1;
						break;
					case InputKeys::RIGHT:
						next_dir.x += 1;
						break;
					case InputKeys::LEFT:
						next_dir.x -= 1;
						break;
					case InputKeys::SLASH:
						handle_soldier_spawn(
							soldier_count_2, wait_2, transform, tile,
							MazeRegion::PLAYER2, TeamType::PLAYER2,
							textures_path("blue_soldier_sprite_sheet-01.png")
						);
						break;
					default:
						break;
					}
				}
			}
			++wait_2;
		}
		motion.direction = next_dir;
	}
}

void PlayerInputSystem::handle_soldier_spawn(
	size_t& soldier_count, size_t& wait_time, const Transform& transform, const Tile& tile,
	const MazeRegion& maze_region, const TeamType& team_type, const char* texture_path
)
{
	if (can_spawn(soldier_count, wait_time, transform, tile, maze_region))
	{
		vec2 position = tile.get_position();
		Transform transform_soldier = Transform{ position, position,{ 0.08f, 0.08f }, position };
		Motion motion_soldier = Motion{ { 0, 0 }, 100.f };
		spawn_soldier(
			transform_soldier, motion_soldier,
			team_type, texture_path
		);
		++soldier_count;
		wait_time = 0;
	}
}

bool PlayerInputSystem::can_spawn(
	const size_t& soldier_count, const size_t& wait_time, const Transform& transform,
	const Tile& tile, const MazeRegion& maze_region
)
{
	const std::pair<int, int> tile_idx = tile.get_idx();
	return (
		(!tile.is_wall()) &&
		(soldier_count < MAX_SOLDIERS) &&
		(wait_time > RESPAWN_IDLE) &&
		(tile_idx.first > 3 && tile_idx.first < 15) &&
		(tile_idx.second > 4 && tile_idx.second < 23) && // Right at the opening edge
		(m_tilemap->get_region(transform.position.x, transform.position.y) == maze_region)
	);
}

void PlayerInputSystem::onKeyListener(InputKeyEvent* input)
{
	keysPressed[input->key] = true;
}

void PlayerInputSystem::onReleaseListener(KeyReleaseEvent* input)
{
	keysPressed[input->keyReleased] = false;
}

void PlayerInputSystem::reset() {
    keysPressed.clear();
}

void PlayerInputSystem::spawn_soldier(
	const Transform& transform, const Motion& motion,
	const TeamType& team_type, const char* texture_path
)
{
	Entity soldier = ecsManager.createEntity();
	ecsManager.addComponent<Team>(soldier, Team{ team_type });
	ecsManager.addComponent<PlaceableComponent>(soldier, PlaceableComponent{});
	ecsManager.addComponent<Transform>(soldier, transform);
	ecsManager.addComponent<Motion>(soldier, motion);

	ecsManager.addComponent<SoldierAiComponent>(soldier, SoldierAiComponent{});

	Effect soldierEffect{};
	soldierEffect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl"));
	ecsManager.addComponent<Effect>(soldier, soldierEffect);
	Sprite soldierSprite = { texture_path };
	TextureManager::instance()->load_from_file(soldierSprite);
	soldierSprite.sprite_index = { 0 , 3 };
	soldierSprite.sprite_size = { soldierSprite.width / 7.0f , soldierSprite.height / 5.0f };
	ecsManager.addComponent<Sprite>(soldier, soldierSprite);
	MeshComponent soldierMesh{};
	soldierMesh.id = MeshManager::instance()-> init_mesh(
		soldierSprite.width, soldierSprite.height, soldierSprite.sprite_size.x, soldierSprite.sprite_size.y,
		soldierSprite.sprite_index.x, soldierSprite.sprite_index.y, 0
	);
	ecsManager.addComponent<MeshComponent>(soldier, soldierMesh);
	ecsManager.addComponent(
		soldier,
		C_Collision{
			CollisionLayer::Enemy,
			soldierSprite.width / 2 * 0.08f,
			{ soldierSprite.width * 0.08f * 0.8f, soldierSprite.height * 0.08f * 0.8f }
		}
	);
}