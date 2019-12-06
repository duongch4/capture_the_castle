#include "bandit_ai_system.hpp"
#include <iostream>
bool BanditAiSystem::init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players)
{
	m_targets = players;
	m_tilemap = tilemap;
	return true;
}

void BanditAiSystem::update(float& elapsed_ms)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		Entity bandit = *it;
		BanditType bandit_type = ecsManager.getComponent<BanditSpawnComponent>(bandit).type;
		BanditAiComponent& bandit_ai_comp = ecsManager.getComponent<BanditAiComponent>(bandit);

		BanditState& state = bandit_ai_comp.state;
		size_t& idle_time = bandit_ai_comp.idle_time;
		size_t& chase_time = bandit_ai_comp.chase_time;
		size_t& patrol_time = bandit_ai_comp.patrol_time;
		vec2& prev_dir = bandit_ai_comp.prev_dir;

		float speed = BASE_SPEED * (1.f + dist(rng));
		ecsManager.getComponent<Motion>(bandit).speed = speed;

		float distance_1 = get_distance(m_targets[0], bandit);
		float distance_2 = get_distance(m_targets[1], bandit);

		if (bandit_type == BanditType::NORM)
		{
			switch (state)
			{
			case BanditState::IDLE:
				handle_idle(state, idle_time, chase_time, distance_1, distance_2, bandit);
				break;
			case BanditState::PATROL:
				handle_patrol(state, patrol_time, chase_time, distance_1, distance_2, bandit, prev_dir);
				break;
			case BanditState::CHASE:
				handle_chase(state, chase_time, distance_1, distance_2, bandit);
				break;
			}
		}
		else if(bandit_type == BanditType::BOSS)
		{
			switch (state)
			{
			case BanditState::IDLE:
				handle_idle_search(state, chase_time, distance_1, distance_2, bandit);
				break;
				//case BanditState::CHASE:
				//	handle_chase(state, chase_time, distance_1, distance_2, bandit);
				//	break;
			case BanditState::SEARCH:
				handle_search(state, bandit);
				break;
			case BanditState::HOP:
				handle_hop(state, chase_time, distance_1, distance_2, bandit, elapsed_ms);
				break;
			}
		}
	}
}

void BanditAiSystem::handle_patrol(
	BanditState& state, size_t& patrol_time, size_t& chase_time,
	const float& distance_1, const float& distance_2,
	const Entity& bandit, vec2& prev_dir
)
{
	if (can_chase(distance_1, distance_2, chase_time))
	{
		state = BanditState::CHASE;
		patrol_time = 0;
		return;
	}

	vec2& curr_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& curr_dir = ecsManager.getComponent<Motion>(bandit).direction;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	if (patrol_time > PATROL_LIMIT)
	{
		state = BanditState::IDLE;
		patrol_time = 0;
		return;
	}

	if (patrol_time == 1 || !can_move(curr_tile))
	{
		std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_nesw(curr_tile);

		std::mt19937 g(rd());
		std::shuffle(adj_tiles.begin(), adj_tiles.end(), g);
		for (auto tile : adj_tiles)
		{
			if (can_move(tile))
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

bool BanditAiSystem::can_move(const Tile& tile)
{
	return is_within_bandit_region(tile) && !tile.is_wall();
}

bool BanditAiSystem::is_within_bandit_region(const Tile& tile)
{
	std::pair<int, int> tile_idx = tile.get_idx();
	return (
		(tile_idx.first > 3 && tile_idx.first < 15) &&
		(tile_idx.second > 4 && tile_idx.second < 23) &&
		m_tilemap->get_region(tile) == MazeRegion::BANDIT
		);
}

void BanditAiSystem::handle_idle(
	BanditState& state, size_t& idle_time, size_t& chase_time,
	const float& distance_1, const float& distance_2, const Entity& bandit
)
{
	if (can_chase(distance_1, distance_2, chase_time))
	{
		state = BanditState::CHASE;
		idle_time = 0;
		return;
	}

	if (idle_time > IDLE_LIMIT)
	{
		state = BanditState::PATROL;
		idle_time = 0;
		return;
	}

	ecsManager.getComponent<Motion>(bandit).direction = { 0, 0 };
	idle_time++;
}

bool BanditAiSystem::can_chase(const float& distance_1, const float& distance_2, size_t& chase_time)
{
	return ((chase_time < CHASE_LIMIT) && (can_chase_target(m_targets[0], distance_1) || can_chase_target(m_targets[1], distance_2)));
}

bool BanditAiSystem::can_chase_target(const Entity& target, const float& distance)
{
	vec2 pos = ecsManager.getComponent<Transform>(target).position;
	return ((distance < CHASE_DISTANCE_THRESHOLD) && (m_tilemap->get_region(pos.x, pos.y) == MazeRegion::BANDIT));
}

void BanditAiSystem::handle_chase(
	BanditState& state,	size_t& chase_time,
	const float& distance_1, const float& distance_2, const Entity& bandit
)
{
	vec2& curr_pos = ecsManager.getComponent<Transform>(bandit).position;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	if (!can_chase(distance_1, distance_2, chase_time) || curr_tile.is_wall())
	{
		state = BanditState::IDLE;
		chase_time = 0;
		return;
	}

	if (can_move(curr_tile) && can_chase(distance_1, distance_2, chase_time))
	{
		chase(distance_1, distance_2, bandit);
	}

	chase_time++;
}

void BanditAiSystem::chase(const float& distance_1, const float& distance_2, const Entity& bandit)
{
	if (distance_1 < distance_2)
	{
		follow_direction(m_targets[0], bandit);
	}
	else
	{
		follow_direction(m_targets[1], bandit);
	}
}

float BanditAiSystem::get_distance(const Entity& target, const Entity& bandit) const
{
	vec2 target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2 bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2 difference = { target_transform_pos.x - bandit_transform_pos.x, target_transform_pos.y - bandit_transform_pos.y };
	return sqrtf((difference.x * difference.x) + (difference.y * difference.y));
}

void BanditAiSystem::follow_direction(const Entity& target, const Entity& bandit)
{
	vec2& target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2& bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& target_motion_dir = ecsManager.getComponent<Motion>(target).direction;
	vec2& bandit_motion_dir = ecsManager.getComponent<Motion>(bandit).direction;
	float& bandit_speed = ecsManager.getComponent<Motion>(bandit).speed;

	if (
		is_target_move_toward_bandit(bandit_transform_pos, target_transform_pos, target_motion_dir) ||
		is_target_move_away_bandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		bandit_speed += SPEED_UP;
	}

	float dir_x = target_transform_pos.x - bandit_transform_pos.x;
	float dir_y = target_transform_pos.y - bandit_transform_pos.y;
	float distance = len(vec2{ dir_x,dir_y }) + 1e-5f;
	bandit_motion_dir = { dir_x / distance, dir_y / distance };
}

bool BanditAiSystem::is_target_move_toward_bandit(
	const vec2& bandit_transform_pos, const vec2& target_transform_pos, const vec2& target_motion_dir
)
{
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y < 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y > 0.f))
		);
}

bool BanditAiSystem::is_target_move_away_bandit(
	const vec2& bandit_transform_pos, const vec2& target_transform_pos, const vec2& target_motion_dir
)
{
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y > 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y < 0.f))
		);
}

bool BanditAiSystem::can_search(Entity target)
{
	vec2 target_pos = ecsManager.getComponent<Transform>(target).position;
	Tile target_tile = m_tilemap->get_tile(target_pos.x, target_pos.y);
	return is_within_bandit_region(target_tile);
}

void BanditAiSystem::handle_idle_search(
	BanditState& state,	size_t& chase_time,
	const float& distance_1, const float& distance_2, const Entity& bandit
)
{
	m_path.clear();
	path_idx = 0;

	//if (can_chase(distance_1, distance_2, chase_time))
	//{
	//	state = State::CHASE;
	//	return;
	//}

	if (can_search(m_targets[0]) || can_search(m_targets[1]))
	{
		state = BanditState::SEARCH;
		return;
	}

	ecsManager.getComponent<Motion>(bandit).direction = { 0, 0 };
}

void BanditAiSystem::handle_search(BanditState& state, Entity& bandit)
{
	m_path.clear();
	path_idx = 0;

	if (!can_search(m_targets[0]) && !can_search(m_targets[1]))
	{
		state = BanditState::IDLE;
		return;
	}

	vec2& bandit_pos = ecsManager.getComponent<Transform>(bandit).position;
	Tile bandit_tile = m_tilemap->get_tile(bandit_pos.x, bandit_pos.y);
	std::vector<std::vector<Tile>> paths;
	for (auto target : m_targets)
	{
		if (can_search(target))
		{
			vec2 target_pos = ecsManager.getComponent<Transform>(target).position;
			Tile target_tile = m_tilemap->get_tile(target_pos.x, target_pos.y);
			paths.emplace_back(init_path_finding(m_tilemap, bandit_tile, target_tile));
		}
	}

	if (paths.size() == 1)
	{
		m_path = paths[0];
	}
	else if (paths.size() == 2)
	{
		if (paths[0].size() > paths[1].size())
		{
			m_path = paths[1];
		}
		else
		{
			m_path = paths[0];
		}
	}

	if (m_path.size() > 1)
	{
		state = BanditState::HOP;
		return;
	}
	else
	{
		state = BanditState::IDLE;
		return;
	}
}


void BanditAiSystem::handle_hop(
	BanditState& state, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& elapsed_ms
)
{
	//if (can_chase(distance_1, distance_2, chase_time))
	//{
	//	state = State::CHASE;
	//	path_idx = 0;
	//	m_path.clear();
	//	return;
	//}

	if (path_idx < m_path.size())
	{
		const Tile tile = m_path[path_idx];
		const vec2 tile_pos = tile.get_position();
		vec2& bandit_pos = ecsManager.getComponent<Transform>(bandit).position;

		const float sec = elapsed_ms / 1000.f;
		m_hop_timer -= sec;
		if (m_hop_timer < 0.f)
		{
			bandit_pos = tile_pos;
			m_hop_timer = HOP_DELAY;
			++path_idx;
		}
	}
	else
	{
		state = BanditState::IDLE;
		path_idx = 0;
		m_path.clear();
	}
}

/* PATH FINDING */

std::vector<Tile> BanditAiSystem::init_path_finding(std::shared_ptr<Tilemap> tilemap, Tile init_tile, Tile goal_tile)
{
	m_tilemap = tilemap;
	m_init_tile = init_tile;
	m_goal_tile = goal_tile;
	return do_BFS();
}

void BanditAiSystem::clear_path_finding()
{
	m_path.clear();
}

std::vector<Tile> BanditAiSystem::do_BFS()
{
	std::pair<int, int> hw = m_tilemap->get_height_width();
	int height = hw.first;
	int width = hw.second;
	// Visited Matrix
	std::vector<std::vector<bool>> V(height, std::vector<bool>(width, false));
	// Parents Matrix
	std::vector<std::vector<Tile>> P(height, std::vector<Tile>(width, m_goal_tile));

	std::queue<Tile> queue;
	queue.push(m_init_tile);

	while (!queue.empty())
	{
		Tile curr = queue.front();
		queue.pop();

		std::vector<Tile> adj_list = m_tilemap->get_adjacent_tiles_nesw(curr);

		for (size_t i = 0; i < adj_list.size(); ++i)
		{
			Tile next = adj_list[i];

			if (is_next_good(next, curr, V))
			{
				std::pair<int, int> next_idx = next.get_idx();
				V[next_idx.first][next_idx.second] = true;
				P[next_idx.first][next_idx.second] = curr;
				queue.push(next);
			}
		}
	}
	return assemble_path(P, m_init_tile, m_goal_tile);
}

bool BanditAiSystem::is_next_good(Tile next, Tile curr, std::vector<std::vector<bool>>& visited_matrix)
{
	return (!is_visited(next, visited_matrix) && is_within_bandit_region(next) && !next.is_wall() && !curr.is_wall());
}

bool BanditAiSystem::is_visited(Tile tile, std::vector<std::vector<bool>>& visited_matrix)
{
	std::pair<int, int> tile_idx = tile.get_idx();
	return visited_matrix[tile_idx.first][tile_idx.second];
}

std::vector<Tile> BanditAiSystem::assemble_path(std::vector<std::vector<Tile>>& parents_matrix, Tile init_tile, Tile goal_tile)
{
	std::vector<Tile> result;
	std::vector<Tile> stack;

	result.emplace_back(init_tile);

	while (!is_equal(goal_tile, init_tile))
	{
		stack.emplace_back(goal_tile);
		std::pair<int, int> goal_idx = goal_tile.get_idx();
		Tile goal_parent = parents_matrix[goal_idx.first][goal_idx.second];
		goal_tile = (is_equal(goal_tile, goal_parent)) ? init_tile : goal_parent;
	}

	if (stack.size() < 2)
	{
		return result;
	}

	while (!stack.empty())
	{
		result.emplace_back(stack.back());
		stack.pop_back();
	}

	return result;
}

bool BanditAiSystem::is_equal(Tile a, Tile b)
{
	return (a.get_idx() == b.get_idx());
}

void BanditAiSystem::reset()
{
	m_targets.clear();
	m_targets.shrink_to_fit();
	m_path.clear();
	m_path.shrink_to_fit();
	m_tilemap->destroy();
	this->entities.clear();
	//this->entities.shrink_to_fit();
	//this->entities = {};
}
