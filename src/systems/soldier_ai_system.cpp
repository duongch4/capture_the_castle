#include "soldier_ai_system.hpp"

bool SoldierAiSystem::init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players)
{
	m_targets = players;
	for (size_t i = 0; i < MAX_SOLDIERS; ++i)
	{
		m_idle_times.emplace_back(0);
		m_patrol_times.emplace_back(0);
		m_prev_dirs.emplace_back(vec2{ 1.f,0.f });
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
		size_t& idle_time = m_idle_times[idx];
		size_t& patrol_time = m_patrol_times[idx];
		vec2& prev_dir = m_prev_dirs[idx];

		float speed = BASE_SPEED * (1.f + dist(rng));
		ecsManager.getComponent<Motion>(soldier).speed = speed;

		switch (state)
		{
		case State::IDLE:
			handle_idle(state, idle_time, soldier);
			break;
		case State::PATROL:
			handle_patrol(state, patrol_time, soldier, prev_dir);
			break;
		}
	}
	return;

}

void SoldierAiSystem::handle_patrol(State& state, size_t& patrol_time, const Entity& soldier, vec2& prev_dir)
{
	vec2& curr_pos = ecsManager.getComponent<Transform>(soldier).position;
	vec2& curr_dir = ecsManager.getComponent<Motion>(soldier).direction;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	if (patrol_time > PATROL_LIMIT)
	{
		curr_pos = curr_tile.get_position();

		state = State::IDLE;
		patrol_time = 0;
		return;
	}
	
	if (patrol_time == 1 || !can_move(soldier, curr_tile))
	{
		std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_nesw(curr_tile);

		std::mt19937 g(rd());
		std::shuffle(adj_tiles.begin(), adj_tiles.end(), g);
		for (auto tile : adj_tiles)
		{
			if (can_move(soldier, tile))
			{
				std::pair<int, int> tile_idx = tile.get_idx();
				std::pair<int, int> curr_idx = curr_tile.get_idx();
				if (tile_idx.first < curr_idx.first) curr_dir = { 0,-1 };
				else if (tile_idx.first > curr_idx.first) curr_dir = { 0,1 };
				else if (tile_idx.second < curr_idx.second) curr_dir = { -1,0 };
				else if (tile_idx.second > curr_idx.second) curr_dir = { 1,0 };
				
				prev_dir = curr_dir;
				break;
			}
		}
	}
	else
	{
		curr_dir = prev_dir;
	}
	patrol_time++;
}

bool SoldierAiSystem::can_move(const Entity& soldier, const Tile& tile)
{
	return is_within_soldier_region(soldier, tile) && !tile.is_wall();
}

void SoldierAiSystem::handle_idle(State& state, size_t& idle_time, const Entity& soldier)
{
	if (idle_time > IDLE_LIMIT)
	{
		state = State::PATROL;
		idle_time = 0;
		return;
	}

	ecsManager.getComponent<Motion>(soldier).direction = { 0, 0 };
	idle_time++;
}

void SoldierAiSystem::follow_direction(Entity& target, Entity& soldier, float& speed, float& elapsed_ms)
{
	vec2& target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2& soldier_transform_pos = ecsManager.getComponent<Transform>(soldier).position;
	vec2& target_motion_dir = ecsManager.getComponent<Motion>(target).direction;
	vec2& soldier_motion_dir = ecsManager.getComponent<Motion>(soldier).direction;

	float step = speed * elapsed_ms / 1000.f;

	if (is_target_move_toward_soldier(soldier_transform_pos, target_transform_pos, target_motion_dir))
	{
		soldier_transform_pos.x -= target_motion_dir.x * step;
		soldier_transform_pos.y -= target_motion_dir.y * step;
	}
	if (is_target_move_away_soldier(soldier_transform_pos, target_transform_pos, target_motion_dir))
	{
		soldier_transform_pos.x += target_motion_dir.x * step;
		soldier_transform_pos.y += target_motion_dir.y * step;
	}

	float dir_x = target_transform_pos.x - soldier_transform_pos.x;
	float dir_y = target_transform_pos.y - soldier_transform_pos.y;
	float distance = len(vec2{ dir_x,dir_y }) + 1e-5f;
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
	std::pair<int, int> tile_idx = tile.get_idx();
	return (
		(tile_idx.first > 3 && tile_idx.first < 15) &&
		(tile_idx.second > 4 && tile_idx.second < 23) &&
		m_tilemap->get_region(tile) == maze_region
	);
}

void SoldierAiSystem::reset() {
    m_idle_times.clear();
    m_patrol_times.clear();
    m_states.clear();
    m_soldiers.clear();
    m_targets.clear();
    m_prev_dirs.clear();
}
