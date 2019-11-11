#include "soldier_ai_system.hpp"

bool SoldierAiSystem::init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players)
{
	m_targets = players;
	for (size_t i = 0; i < MAX_SOLDIERS; ++i)
	{
		m_idle_times.emplace_back(0);
		m_chase_times.emplace_back(0);
		m_patrol_times.emplace_back(0);
		m_prev_dirs.emplace_back(vec2{1.f,0.f});
		m_states.emplace_back(State::IDLE);
	}
	m_tilemap = tilemap;
	return true;
}

void SoldierAiSystem::update(float& elapsed_ms)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		auto idx = std::distance(entities.begin(), it);
		Entity soldier = *it;
		State& state = m_states[idx];
		size_t& idleTime = m_idle_times[idx];
		size_t& chaseTime = m_chase_times[idx];
		size_t& patrolTime = m_patrol_times[idx];
		vec2& prev_dir = m_prev_dirs[idx];

		float speed = BASE_SPEED * (1.f + dist(rng));
		ecsManager.getComponent<Motion>(soldier).speed = speed;

		TeamType team = ecsManager.getComponent<Team>(soldier).assigned;
		//std::cout << speed << std::endl;

		float distance_1 = get_distance(m_targets[0], soldier);
		float distance_2 = get_distance(m_targets[1], soldier);


		////vec2 pos = ecsManager.getComponent<Transform>(m_targets[0]).position;
		////Tile curr = m_tilemap->get_tile(pos.x, pos.y);
		////std::cout << "player:::" << curr.get_idx().first << ":" << curr.get_idx().second << std::endl;

		////vec2 b_pos = ecsManager.getComponent<Transform>(soldier).position;
		////Tile b_curr = m_tilemap->get_tile(b_pos.x, b_pos.y);
		////std::cout << "soldier:::" << b_curr.get_idx().first << ":" << b_curr.get_idx().second << std::endl;

		switch (state)
		{
		case State::IDLE:
			handle_idle(state, idleTime, patrolTime, distance_1, distance_2, soldier, speed, elapsed_ms);
			break;
		case State::PATROL:
			handle_patrol(state, idleTime, patrolTime, distance_1, distance_2, soldier, speed, elapsed_ms, prev_dir);
			break;
		}
	}
	return;

}

void SoldierAiSystem::handle_patrol(
	State& state, size_t& idle_time, size_t& patrol_time,
	float& distance_1, float& distance_2,
	Entity& soldier, float& speed, float& elapsed_ms, vec2& prev_dir
)
{
	if (patrol_time > PATROL_LIMIT)
	{
		state = State::IDLE;
		patrol_time = 0;
		return;
	}

	vec2& curr_pos = ecsManager.getComponent<Transform>(soldier).position;
	vec2& curr_dir = ecsManager.getComponent<Motion>(soldier).direction;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	//std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_nesw(curr_pos.x, curr_pos.y);
	//Tile next_tile;
	//if (curr_dir.y < 0) next_tile = adj_tiles[0]; // north
	//else if (curr_dir.x > 0) next_tile = adj_tiles[1]; // east
	//else if (curr_dir.y > 0) next_tile = adj_tiles[2]; // south
	//else if (curr_dir.x < 0) next_tile = adj_tiles[3]; // west
	//else next_tile = curr_tile;

	if (patrol_time == 1 || !is_within_soldier_region(soldier, curr_tile) || curr_tile.is_wall())
	{
		std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_nesw(curr_pos.x, curr_pos.y);

		std::mt19937 g(rd());
		std::shuffle(adj_tiles.begin(), adj_tiles.end(), g);
		for (auto tile : adj_tiles)
		{
			if (is_within_soldier_region(soldier, tile) && !tile.is_wall())
			{
				//vec2 next_pos = tile.get_position();
				//float dir_x = next_pos.x - curr_pos.x;
				//float dir_y = next_pos.y - curr_pos.y;
				//float distance = std::sqrtf((dir_x * dir_x) + (dir_y * dir_y)) + 1e-5f;
				//curr_dir = { (dir_x / distance), (dir_y / distance) };

				std::pair<int, int> tile_idx = tile.get_idx();
				std::pair<int, int> curr_idx = curr_tile.get_idx();
				if (tile_idx.first < curr_idx.first) curr_dir = { -1,0 };
				else if (tile_idx.first > curr_idx.first) curr_dir = { 1,0 };
				else if (tile_idx.second < curr_idx.second) curr_dir = { 0,-1 };
				else if (tile_idx.second > curr_idx.second) curr_dir = { 0,1 };
				
				prev_dir = curr_dir;
				break;
			}
		}
	}
	else
	{
		curr_dir = prev_dir;
	}
	
	//if (patrol_time == 1 || !is_within_soldier_region(curr_tile) || curr_tile.is_wall())
	//{
	//	//std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_wesn(curr_pos.x, curr_pos.y);
	//	std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_wesn(curr_pos.x, curr_pos.y);

	//	std::mt19937 g(rd());
	//	std::shuffle(adj_tiles.begin(), adj_tiles.end(), g);

	//	for (auto tile : adj_tiles)
	//	{
	//		if (is_within_soldier_region(tile) && !tile.is_wall())
	//		{
	//			vec2 next_pos = tile.get_position();
	//			float dir_x = next_pos.x - curr_pos.x;
	//			float dir_y = next_pos.y - curr_pos.y;
	//			float distance = std::sqrtf((dir_x * dir_x) + (dir_y * dir_y)) + 1e-5f;
	//			curr_dir = { (dir_x / distance), (dir_y / distance) };

	//			//std::pair<int, int> tile_idx = tile.get_idx();
	//			//std::pair<int, int> curr_idx = curr_tile.get_idx();
	//			//if (tile_idx.first < curr_idx.first) curr_dir = { -1,0 };
	//			//if (tile_idx.first > curr_idx.first) curr_dir = { 1,0 };
	//			//if (tile_idx.second < curr_idx.second) curr_dir = { 0,-1 };
	//			//if (tile_idx.second > curr_idx.second) curr_dir = { 0,1 };
	//			break;
	//		}
	//	}
	//}
	patrol_time++;
}

void SoldierAiSystem::handle_idle(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& soldier, float& speed, float& elapsed_ms
)
{
	if (idle_time > IDLE_LIMIT)
	{
		//std::cout << "d" << std::endl;

		state = State::PATROL;
		idle_time = 0;
		return;
	}
	//std::cout << "e" << std::endl;

	ecsManager.getComponent<Motion>(soldier).direction = { 0, 0 };
	idle_time++;
}

float SoldierAiSystem::get_distance(Entity& target, Entity& soldier)
{
	vec2 target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2 soldier_transform_pos = ecsManager.getComponent<Transform>(soldier).position;
	vec2 difference = { target_transform_pos.x - soldier_transform_pos.x, target_transform_pos.y - soldier_transform_pos.y };
	return sqrtf((difference.x * difference.x) + (difference.y * difference.y));
}

void SoldierAiSystem::follow_direction(Entity& target, Entity& soldier, float& speed, float& elapsed_ms)
{
	vec2& target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2& soldier_transform_pos = ecsManager.getComponent<Transform>(soldier).position;
	vec2& target_motion_dir = ecsManager.getComponent<Motion>(target).direction;
	vec2& soldier_motion_dir = ecsManager.getComponent<Motion>(soldier).direction;

	float step = speed * elapsed_ms / 1000.f;

	if (
		is_target_move_toward_soldier(soldier_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		soldier_transform_pos.x -= target_motion_dir.x * step;
		soldier_transform_pos.y -= target_motion_dir.y * step;
	}
	if (
		is_target_move_away_soldier(soldier_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		soldier_transform_pos.x += target_motion_dir.x * step;
		soldier_transform_pos.y += target_motion_dir.y * step;
	}

	float distance = get_distance(target, soldier) + 1e-5f;
	float dir_x = target_transform_pos.x - soldier_transform_pos.x;
	float dir_y = target_transform_pos.y - soldier_transform_pos.y;
	soldier_motion_dir = { dir_x / distance, dir_y / distance };
}

bool SoldierAiSystem::is_target_move_toward_soldier(
	vec2& soldier_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir
)
{
	return (
		((soldier_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((soldier_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((soldier_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y < 0.f)) ||
		((soldier_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y > 0.f))
		);
}

bool SoldierAiSystem::is_target_move_away_soldier(
	vec2& soldier_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir
)
{
	return (
		((soldier_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((soldier_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((soldier_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y > 0.f)) ||
		((soldier_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y < 0.f))
		);
}

bool SoldierAiSystem::is_within_soldier_region(const Entity& soldier, const Tile& tile)
{
	TeamType team = ecsManager.getComponent<Team>(soldier).assigned;
	MazeRegion maze_region = (team == TeamType::PLAYER1) ? MazeRegion::PLAYER1 : MazeRegion::PLAYER2;
	vec2 tile_pos = tile.get_position();
	std::pair<int, int> tile_idx = tile.get_idx();
	return (
		(tile_idx.first > 3 && tile_idx.first < 15) &&
		(tile_idx.second > 4 && tile_idx.second < 23) &&
		m_tilemap->get_region(tile_pos.x, tile_pos.y) == maze_region
	);
}
