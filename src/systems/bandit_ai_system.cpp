#include "bandit_ai_system.hpp"

bool BanditAISystem::init(std::shared_ptr<Tilemap> tilemap, Entity& player_1, Entity& player_2)

{

	m_targets.emplace_back(player_1);
	m_targets.emplace_back(player_2);
	for (size_t i = 0; i < MAX_BANDITS; ++i)
	{
		m_idle_times.emplace_back(0);
		m_chase_times.emplace_back(0);
		m_states.emplace_back(State::IDLE);
	}
	m_tilemap = tilemap;
	return true;
}

void BanditAISystem::update(float& elapsed_ms)
{
	for (auto it = entities.begin(); it != entities.end(); ++it)
	{
		auto idx = std::distance(entities.begin(), it);
		Entity bandit = *it;
		State& state = m_states[idx];
		size_t& idleTime = m_idle_times[idx];
		size_t& chaseTime = m_chase_times[idx];
		float speed = BASE_SPEED * (1.f + dist(rng));
		ecsManager.getComponent<Motion>(bandit).speed = speed;
		//std::cout << speed << std::endl;

		float distance_1 = get_distance(m_targets[0], bandit);
		float distance_2 = get_distance(m_targets[1], bandit);


		//vec2 pos = ecsManager.getComponent<Transform>(m_targets[0]).position;
		//Tile curr = m_tilemap->get_tile(pos.x, pos.y);
		//std::cout << "player:::" << curr.get_idx().first << ":" << curr.get_idx().second << std::endl;

		//vec2 b_pos = ecsManager.getComponent<Transform>(bandit).position;
		//Tile b_curr = m_tilemap->get_tile(b_pos.x, b_pos.y);
		//std::cout << "bandit:::" << b_curr.get_idx().first << ":" << b_curr.get_idx().second << std::endl;

		switch (state)
		{
		case State::IDLE:
			handle_idle(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
			break;
		case State::CHASE:
			handle_chase(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
			break;
		case State::SEARCH:
			handle_search(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
			break;
		case State::PATROL:
			handle_patrol(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
			break;
		}
		//check(state, idleTime, chaseTime, distance_1, distance_2, bandit, speed, elapsed_ms);
	}
}

void BanditAISystem::handle_patrol(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& speed, float& elapsed_ms
)
{
	if (chase_time > CHASE_LIMIT)
	{
		state = State::IDLE;
		idle_time = 0;
		return;
	}
	if (can_chase(distance_1, distance_2, idle_time))
	{
		state = State::CHASE;
		chase_time = 0;
		return;
	}

	//if (can_search(m_targets[0]) || can_search(m_targets[1]))
	//{
	//	state = State::SEARCH;
	//	return;
	//}

	vec2& curr_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& curr_dir = ecsManager.getComponent<Motion>(bandit).direction;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);

	if (chase_time == 1 || !is_within_bandit_region(curr_tile) || curr_tile.is_wall())
	{
		//std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles_wesn(curr_pos.x, curr_pos.y);
		std::vector<Tile> adj_tiles = m_tilemap->get_adjacent_tiles(curr_pos.x, curr_pos.y);

		std::mt19937 g(rd());
		std::shuffle(adj_tiles.begin(), adj_tiles.end(), g);

		for (auto tile : adj_tiles)
		{
			if (is_within_bandit_region(tile) && !tile.is_wall())
			{
				vec2 next_pos = tile.get_position();
				float dir_x = next_pos.x - curr_pos.x;
				float dir_y = next_pos.y - curr_pos.y;
				float distance = sqrtf((dir_x * dir_x) + (dir_y * dir_y)) + 1e-5f;
				curr_dir = { (dir_x / distance), (dir_y / distance) };

				//std::pair<int, int> tile_idx = tile.get_idx();
				//std::pair<int, int> curr_idx = curr_tile.get_idx();
				//if (tile_idx.first < curr_idx.first) curr_dir = { -1,0 };
				//if (tile_idx.first > curr_idx.first) curr_dir = { 1,0 };
				//if (tile_idx.second < curr_idx.second) curr_dir = { 0,-1 };
				//if (tile_idx.second > curr_idx.second) curr_dir = { 0,1 };
				break;
			}
		}
	}
	chase_time++;
}

void BanditAISystem::handle_idle(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& speed, float& elapsed_ms
)
{
	//std::cout << "a:" << idle_time << std::endl;
	if (can_chase(distance_1, distance_2, idle_time))
	{
		//std::cout << "b" << std::endl;

		state = State::CHASE;
		chase_time = 0;
		return;
	}
	//std::cout << "c" << std::endl;

	//if (can_search(m_targets[0]) || can_search(m_targets[1]))
	//{
	//	state = State::SEARCH;
	//	return;
	//}

	if (idle_time > IDLE_LIMIT)
	{
		//std::cout << "d" << std::endl;

		state = State::PATROL;
		chase_time = 0;
		return;
	}
	//std::cout << "e" << std::endl;

	ecsManager.getComponent<Motion>(bandit).direction = { 0, 0 };
	idle_time++;
}

bool BanditAISystem::can_chase(float& distance_1, float& distance_2, size_t& idle_time)
{
	vec2 pos_1 = ecsManager.getComponent<Transform>(m_targets[0]).position;
	vec2 pos_2 = ecsManager.getComponent<Transform>(m_targets[1]).position;
	return (
		((distance_1 < CHASE_THRESHOLD) || (distance_2 < CHASE_THRESHOLD)) &&
		(idle_time > IDLE_LIMIT) &&
		(
			(m_tilemap->get_region(pos_1.x, pos_1.y) == MazeRegion::BANDIT) || 
			(m_tilemap->get_region(pos_2.x, pos_2.y) == MazeRegion::BANDIT)
		)
		);
}

bool BanditAISystem::can_search(Entity target)
{
	vec2 target_pos = ecsManager.getComponent<Transform>(target).position;
	Tile target_tile = m_tilemap->get_tile(target_pos.x, target_pos.y);
	std::pair<int,int> target_tile_idx = target_tile.get_idx();
	return is_within_bandit_region(target_tile_idx.first, target_tile_idx.second, target_pos);
}

void BanditAISystem::handle_chase(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& speed, float& elapsed_ms
)
{
	//std::cout << chase_time << std::endl;
	if (cannot_chase(distance_1, distance_2, chase_time))
	{
		//if (can_search(m_targets[0]) || can_search(m_targets[1]))
		//{
		//	state = State::SEARCH;
		//	return;
		//}
		state = State::IDLE;
		idle_time = 0;
		return;
	}
	//std::cout << "aaa" << std::endl;

	vec2& curr_pos = ecsManager.getComponent<Transform>(bandit).position;
	Tile curr_tile = m_tilemap->get_tile(curr_pos.x, curr_pos.y);
	if (is_within_bandit_region(curr_tile) && !curr_tile.is_wall())
	{
		chase(distance_1, distance_2, bandit, speed, elapsed_ms);
	}
	//std::cout << "bbb" << std::endl;

	chase_time++;
}

bool BanditAISystem::cannot_chase(float& distance_1, float& distance_2, size_t& chase_time)
{
	vec2 pos_1 = ecsManager.getComponent<Transform>(m_targets[0]).position;
	vec2 pos_2 = ecsManager.getComponent<Transform>(m_targets[1]).position;
	return (
		((distance_1 > CHASE_THRESHOLD) && (distance_2 > CHASE_THRESHOLD)) ||
		(chase_time > CHASE_LIMIT) ||
		(
			(m_tilemap->get_region(pos_1.x, pos_1.y) != MazeRegion::BANDIT) &&
			(m_tilemap->get_region(pos_2.x, pos_2.y) != MazeRegion::BANDIT)
			)
		);
}

void BanditAISystem::chase(float& distance_1, float& distance_2, Entity& bandit, float& speed, float& elapsed_ms)
{
	if (distance_1 < distance_2)
	{
		follow_direction(m_targets[0], bandit, speed, elapsed_ms);
	}
	else
	{
		follow_direction(m_targets[1], bandit, speed, elapsed_ms);
	}
}

float BanditAISystem::get_distance(Entity& target, Entity& bandit)
{
	vec2 target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2 bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2 difference = { target_transform_pos.x - bandit_transform_pos.x, target_transform_pos.y - bandit_transform_pos.y };
	return sqrtf((difference.x * difference.x) + (difference.y * difference.y));
}

void BanditAISystem::follow_direction(Entity& target, Entity& bandit, float& speed, float& elapsed_ms)
{
	vec2& target_transform_pos = ecsManager.getComponent<Transform>(target).position;
	vec2& bandit_transform_pos = ecsManager.getComponent<Transform>(bandit).position;
	vec2& target_motion_dir = ecsManager.getComponent<Motion>(target).direction;
	vec2& bandit_motion_dir = ecsManager.getComponent<Motion>(bandit).direction;

	float step = speed * elapsed_ms / 1000.f;

	if (
		is_target_move_toward_bandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		bandit_transform_pos.x -= target_motion_dir.x * step;
		bandit_transform_pos.y -= target_motion_dir.y * step;
	}
	if (
		is_target_move_away_bandit(bandit_transform_pos, target_transform_pos, target_motion_dir)
		)
	{
		bandit_transform_pos.x += target_motion_dir.x * step;
		bandit_transform_pos.y += target_motion_dir.y * step;
	}

	float distance = get_distance(target, bandit) + 1e-5f;
	float dir_x = target_transform_pos.x - bandit_transform_pos.x;
	float dir_y = target_transform_pos.y - bandit_transform_pos.y;
	bandit_motion_dir = { dir_x / distance, dir_y / distance };
}

bool BanditAISystem::is_target_move_toward_bandit(
	vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir
)
{
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y < 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y > 0.f))
		);
}

bool BanditAISystem::is_target_move_away_bandit(
	vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir
)
{
	return (
		((bandit_transform_pos.x < target_transform_pos.x) && (target_motion_dir.x > 0.f)) ||
		((bandit_transform_pos.x > target_transform_pos.x) && (target_motion_dir.x < 0.f)) ||
		((bandit_transform_pos.y < target_transform_pos.y) && (target_motion_dir.y > 0.f)) ||
		((bandit_transform_pos.y > target_transform_pos.y) && (target_motion_dir.y < 0.f))
		);
}

void BanditAISystem::handle_search(
	State& state, size_t& idle_time, size_t& chase_time,
	float& distance_1, float& distance_2,
	Entity& bandit, float& speed, float& elapsed_ms
)
{
	//state = State::CHASE;
	//chase_time = 0;
	//return;

	if (can_chase(distance_1, distance_2, idle_time))
	{
		state = State::CHASE;
		chase_time = 0;
		return;
	}
	
	if (!can_search(m_targets[0]) && !can_search(m_targets[1]))
	{
		state = State::IDLE;
		idle_time = 0;
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
		state = State::PATROL;
		chase_time = 0;
		return;
	}
	else
	{
		state = State::IDLE;
		idle_time = 0;
		return;
	}

	//move_on_path(m_path, speed, elapsed_ms, bandit_pos);
	//clear_path_finding();
}

// TODO - FIXME
void BanditAISystem::move_on_path(std::vector<Tile> path, float& speed, float& elapsed_ms, vec2& bandit_pos)
{
	float step = speed * elapsed_ms / 1000;
	vec2 prev_pos = bandit_pos;
	//for (auto tile : path)
	//{
	if (path_idx < path.size())
	{
		Tile tile = path[path_idx++];
		vec2 tile_pos = tile.get_position();
		bandit_pos = tile_pos;

		//float dir_x = tile_pos.x - prev_pos.x;
		//float dir_y = tile_pos.y - prev_pos.y;
		//float distance = std::sqrtf((dir_x * dir_x) + (dir_y * dir_y)) + 1e-5f;

		//bandit_pos = { dir_x / distance , dir_y / distance };

		//bandit_pos.x += dir_x / distance * step;
		//bandit_pos.y += dir_y / distance * step;

		//prev_pos = tile_pos;
	}
	else
	{
		path_idx = 0;
		path.clear();
	}

	//}
}

/* PATH FINDING */

std::vector<Tile> BanditAISystem::init_path_finding(std::shared_ptr<Tilemap> tilemap, Tile init_tile, Tile goal_tile)
{
	m_tilemap = tilemap;
	m_init_tile = init_tile;
	m_goal_tile = goal_tile;
	return do_BFS();
}

void BanditAISystem::clear_path_finding()
{
	m_path.clear();
}

std::vector<Tile> BanditAISystem::do_BFS()
{
	std::pair<int,int> hw = m_tilemap->get_height_width();
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

		std::vector<Tile> adj_list = m_tilemap->get_adjacent_tiles(curr.get_position().x, curr.get_position().y);

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

bool BanditAISystem::is_next_good(Tile next, Tile curr, std::vector<std::vector<bool>>& visited_matrix)
{
	return (!is_visited(next, visited_matrix) && is_within_bandit_region(next) && !next.is_wall() && !curr.is_wall());
}

bool BanditAISystem::is_visited(Tile tile, std::vector<std::vector<bool>>& visited_matrix)
{
	std::pair<int, int> tile_idx = tile.get_idx();
	//std::cout << "Tile idx::" << (tile_idx.first < visited_matrix.size()) << "," << (tile_idx.second < visited_matrix[0].size()) << std::endl;

	return visited_matrix[tile_idx.first][tile_idx.second];
}

bool BanditAISystem::is_within_bandit_region(Tile tile)
{
	vec2 tile_pos = tile.get_position();
	std::pair<int, int> tile_idx = tile.get_idx();
	return is_within_bandit_region(tile_idx.first, tile_idx.second, tile_pos);
}

bool BanditAISystem::is_within_bandit_region(int idx_row, int idx_col, vec2 pos)
{
	return (
		idx_row > 3 && idx_row < 15 &&
		idx_col > 5 && idx_col < 18 &&
		m_tilemap->get_region(pos.x, pos.y) == MazeRegion::BANDIT
		);
}

std::vector<Tile> BanditAISystem::assemble_path(std::vector<std::vector<Tile>>& parents_matrix, Tile init_tile, Tile goal_tile)
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

bool BanditAISystem::is_equal(Tile a, Tile b)
{
	return (a.get_idx() == b.get_idx());
}

void BanditAISystem::reset() {
    m_idle_times.clear();
    m_chase_times.clear();
    m_states.clear();
    m_bandits.clear();
    m_targets.clear();
    m_path.clear();
}
