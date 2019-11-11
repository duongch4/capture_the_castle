#include "bandit_ai_system.hpp"

bool BanditAiSystem::init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players)
{
	m_targets = players;
	for (size_t i = 0; i < MAX_BANDITS; ++i)
	{
		m_idle_times.emplace_back(0);
		m_chase_times.emplace_back(0);
		m_patrol_times.emplace_back(0);
		m_prev_dirs.emplace_back(vec2{ 1.f,0.f });
		m_states.emplace_back(State::IDLE);
	}
	m_tilemap = tilemap;
	return true;
}

void BanditAiSystem::update(float& elapsed_ms)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		auto idx = std::distance(entities.begin(), it);
		Entity bandit = *it;

		State& state = m_states[idx];
		size_t& idle_time = m_idle_times[idx];
		size_t& chase_time = m_chase_times[idx];
		size_t& patrol_time = m_patrol_times[idx];
		vec2& prev_dir = m_prev_dirs[idx];

		float speed = BASE_SPEED * (1.f + dist(rng));
		ecsManager.getComponent<Motion>(bandit).speed = speed;

		float distance_1 = get_distance(m_targets[0], bandit);
		float distance_2 = get_distance(m_targets[1], bandit);

		switch (state)
		{
		case State::IDLE:
			handle_idle(state, idle_time, chase_time, distance_1, distance_2, bandit);
			break;
		case State::PATROL:
			handle_patrol(state, patrol_time, chase_time, distance_1, distance_2, bandit, prev_dir);
			break;
		case State::CHASE:
			handle_chase(state, chase_time, distance_1, distance_2, bandit);
			break;
		//case State::SEARCH:
		//	handle_search(state, idle_time, chase_time, distance_1, distance_2, bandit, speed, elapsed_ms);
		//	break;

		}
	}
}

void BanditAiSystem::handle_patrol(
	State& state, size_t& patrol_time, size_t& chase_time,
	const float& distance_1, const float& distance_2,
	const Entity& bandit, vec2& prev_dir
)
{
	if (can_chase(distance_1, distance_2, chase_time))
	{
		state = State::CHASE;
		patrol_time = 0;
		return;
	}

	vec2& curr_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& curr_dir = ecsManager.getComponent<Motion>(bandit).direction;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	if (patrol_time > PATROL_LIMIT)
	{
		curr_pos = curr_tile.get_position();

		state = State::IDLE;
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
	State& state, size_t& idle_time, size_t& chase_time,
	const float& distance_1, const float& distance_2, const Entity& bandit
)
{
	if (can_chase(distance_1, distance_2, chase_time))
	{
		state = State::CHASE;
		idle_time = 0;
		return;
	}

	//if (can_search(m_targets[0]) || can_search(m_targets[1]))
	//{
	//	state = State::SEARCH;
	//	return;
	//}

	if (idle_time > IDLE_LIMIT)
	{
		state = State::PATROL;
		idle_time = 0;
		return;
	}

	ecsManager.getComponent<Motion>(bandit).direction = { 0, 0 };
	idle_time++;
}

bool BanditAiSystem::can_chase(const float& distance_1, const float& distance_2, size_t& chase_time)
{
	vec2 pos_1 = ecsManager.getComponent<Transform>(m_targets[0]).position;
	vec2 pos_2 = ecsManager.getComponent<Transform>(m_targets[1]).position;
	return (
		((distance_1 < CHASE_DISTANCE_THRESHOLD) || (distance_2 < CHASE_DISTANCE_THRESHOLD)) &&
		(chase_time < CHASE_LIMIT) &&
		(
			(m_tilemap->get_region(pos_1.x, pos_1.y) == MazeRegion::BANDIT) || 
			(m_tilemap->get_region(pos_2.x, pos_2.y) == MazeRegion::BANDIT)
		)
		);
}

//bool BanditAiSystem::can_search(Entity target)
//{
//	vec2 target_pos = ecsManager.getComponent<Transform>(target).position;
//	Tile target_tile = m_tilemap->get_tile(target_pos.x, target_pos.y);
//	std::pair<int,int> target_tile_idx = target_tile.get_idx();
//	return is_within_bandit_region(target_tile_idx.first, target_tile_idx.second, target_pos);
//}

void BanditAiSystem::handle_chase(
	State& state, size_t& chase_time,
	const float& distance_1, const float& distance_2, const Entity& bandit
)
{
	vec2& curr_pos = ecsManager.getComponent<Transform>(bandit).position;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	if (cannot_chase(distance_1, distance_2, chase_time))
	{
		//if (can_search(m_targets[0]) || can_search(m_targets[1]))
		//{
		//	state = State::SEARCH;
		//	return;
		//}
		state = State::IDLE;
		chase_time = 0;
		return;
	}
	
	if (can_move(curr_tile) && can_chase(distance_1, distance_2, chase_time))
	{
		chase(distance_1, distance_2, bandit);
	}
	//chase(distance_1, distance_2, bandit, speed, elapsed_ms);
	chase_time++;
}

bool BanditAiSystem::cannot_chase(const float& distance_1, const float& distance_2, size_t& chase_time)
{
	vec2 pos_1 = ecsManager.getComponent<Transform>(m_targets[0]).position;
	vec2 pos_2 = ecsManager.getComponent<Transform>(m_targets[1]).position;
	return (
		((distance_1 > CHASE_DISTANCE_THRESHOLD) && (distance_2 > CHASE_DISTANCE_THRESHOLD)) ||
		(chase_time > CHASE_LIMIT) ||
		(
			(m_tilemap->get_region(pos_1.x, pos_1.y) != MazeRegion::BANDIT) &&
			(m_tilemap->get_region(pos_2.x, pos_2.y) != MazeRegion::BANDIT)
			)
		);
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

//void BanditAiSystem::handle_search(
//	State& state, size_t& idle_time, size_t& chase_time,
//	float& distance_1, float& distance_2,
//	Entity& bandit, float& speed, float& elapsed_ms
//)
//{
//	//state = State::CHASE;
//	//chase_time = 0;
//	//return;
//
//	if (can_chase(distance_1, distance_2, idle_time))
//	{
//		state = State::CHASE;
//		chase_time = 0;
//		return;
//	}
//	
//	if (!can_search(m_targets[0]) && !can_search(m_targets[1]))
//	{
//		state = State::IDLE;
//		idle_time = 0;
//		return;
//	}
//
//	vec2& bandit_pos = ecsManager.getComponent<Transform>(bandit).position;
//	Tile bandit_tile = m_tilemap->get_tile(bandit_pos.x, bandit_pos.y);
//	std::vector<std::vector<Tile>> paths;
//	for (auto target : m_targets)
//	{
//		if (can_search(target))
//		{
//			vec2 target_pos = ecsManager.getComponent<Transform>(target).position;
//			Tile target_tile = m_tilemap->get_tile(target_pos.x, target_pos.y);
//			paths.emplace_back(init_path_finding(m_tilemap, bandit_tile, target_tile));
//		}
//	}
//
//	if (paths.size() == 1)
//	{
//		m_path = paths[0];
//	}
//	else if (paths.size() == 2)
//	{
//		if (paths[0].size() > paths[1].size())
//		{
//			m_path = paths[1];
//		}
//		else
//		{
//			m_path = paths[0];
//		}
//	}
//
//	if (m_path.size() > 1)
//	{
//		state = State::PATROL;
//		chase_time = 0;
//		return;
//	}
//	else
//	{
//		state = State::IDLE;
//		idle_time = 0;
//		return;
//	}
//
//	//move_on_path(m_path, speed, elapsed_ms, bandit_pos);
//	//clear_path_finding();
//}
//
//// TODO - FIXME
//void BanditAiSystem::move_on_path(std::vector<Tile> path, float& speed, float& elapsed_ms, vec2& bandit_pos)
//{
//	float step = speed * elapsed_ms / 1000;
//	vec2 prev_pos = bandit_pos;
//	//for (auto tile : path)
//	//{
//	if (path_idx < path.size()) 
//	{
//		Tile tile = path[path_idx++];
//		vec2 tile_pos = tile.get_position();
//		bandit_pos = tile_pos;
//
//		//float dir_x = tile_pos.x - prev_pos.x;
//		//float dir_y = tile_pos.y - prev_pos.y;
//		//float distance = std::sqrtf((dir_x * dir_x) + (dir_y * dir_y)) + 1e-5f;
//
//		//bandit_pos = { dir_x / distance , dir_y / distance };
//
//		//bandit_pos.x += dir_x / distance * step;
//		//bandit_pos.y += dir_y / distance * step;
//
//		//prev_pos = tile_pos;
//	}
//	else
//	{
//		path_idx = 0;
//		path.clear();
//	}
//
//	//}
//}
//
///* PATH FINDING */
//
//std::vector<Tile> BanditAiSystem::init_path_finding(std::shared_ptr<Tilemap> tilemap, Tile init_tile, Tile goal_tile)
//{
//	m_tilemap = tilemap;
//	m_init_tile = init_tile;
//	m_goal_tile = goal_tile;
//	return do_BFS();
//}
//
//void BanditAiSystem::clear_path_finding()
//{
//	m_path.clear();
//}
//
//std::vector<Tile> BanditAiSystem::do_BFS()
//{
//	std::pair<int,int> hw = m_tilemap->get_height_width();
//	int height = hw.first;
//	int width = hw.second;
//	// Visited Matrix
//	std::vector<std::vector<bool>> V(height, std::vector<bool>(width, false));
//	// Parents Matrix
//	std::vector<std::vector<Tile>> P(height, std::vector<Tile>(width, m_goal_tile));
//
//	std::queue<Tile> queue;
//	queue.push(m_init_tile);
//
//	while (!queue.empty())
//	{
//		Tile curr = queue.front();
//		queue.pop();
//
//		std::vector<Tile> adj_list = m_tilemap->get_adjacent_tiles(curr.get_position().x, curr.get_position().y);
//
//		for (size_t i = 0; i < adj_list.size(); ++i) 
//		{
//			Tile next = adj_list[i];
//
//			if (is_next_good(next, curr, V))
//			{
//				std::pair<int, int> next_idx = next.get_idx();
//				V[next_idx.first][next_idx.second] = true;
//				P[next_idx.first][next_idx.second] = curr;
//				queue.push(next);
//			}
//		}
//	}
//	return assemble_path(P, m_init_tile, m_goal_tile);
//}
//
//bool BanditAiSystem::is_next_good(Tile next, Tile curr, std::vector<std::vector<bool>>& visited_matrix)
//{
//	return (!is_visited(next, visited_matrix) && is_within_bandit_region(next) && !next.is_wall() && !curr.is_wall());
//}
//
//bool BanditAiSystem::is_visited(Tile tile, std::vector<std::vector<bool>>& visited_matrix)
//{
//	std::pair<int, int> tile_idx = tile.get_idx();
//	//std::cout << "Tile idx::" << (tile_idx.first < visited_matrix.size()) << "," << (tile_idx.second < visited_matrix[0].size()) << std::endl;
//
//	return visited_matrix[tile_idx.first][tile_idx.second];
//}

//std::vector<Tile> BanditAiSystem::assemble_path(std::vector<std::vector<Tile>>& parents_matrix, Tile init_tile, Tile goal_tile)
//{
//	std::vector<Tile> result;
//	std::vector<Tile> stack;
//
//	result.emplace_back(init_tile);
//
//	while (!is_equal(goal_tile, init_tile))
//	{
//		stack.emplace_back(goal_tile);
//		std::pair<int, int> goal_idx = goal_tile.get_idx();
//		Tile goal_parent = parents_matrix[goal_idx.first][goal_idx.second];
//		goal_tile = (is_equal(goal_tile, goal_parent)) ? init_tile : goal_parent;
//	}
//
//	if (stack.size() < 2)
//	{
//		return result;
//	}
//
//	while (!stack.empty())
//	{
//		result.emplace_back(stack.back());
//		stack.pop_back();
//	}
//
//	return result;
//}
//
//bool BanditAiSystem::is_equal(Tile a, Tile b)
//{
//	return (a.get_idx() == b.get_idx());
//}