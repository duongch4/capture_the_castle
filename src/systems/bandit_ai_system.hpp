//
// Created by Owner on 2019-10-16.
//

#ifndef CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP

#include <memory>
#include <random>

#include <iostream>

#include <systems/bandit_spawn_system.hpp>

#include <ecs/common_ecs.hpp>
#include <ecs/ecs_manager.hpp>
#include <components.hpp>
#include <tilemap.hpp>

extern ECSManager ecsManager;

class BanditAiSystem : public System
{
public:
	bool init(std::shared_ptr<Tilemap> tilemap, const std::vector<Entity>& players);
	void update(float& ms);

private:
	enum struct State
	{
		IDLE,
		CHASE,
		SEARCH,
		PATROL,
		OUT_OF_BOUND
	};

	const size_t MAX_BANDITS = BanditSpawnSystem::get_max_bandits();
	const size_t IDLE_LIMIT = 70; // beats
	const size_t CHASE_THRESHOLD = 100;
	const size_t CHASE_LIMIT = 120; // beats
	//const size_t PATROL_LIMIT = 20;
	const float BASE_SPEED = 40.f;

	std::vector<size_t> m_idle_times;
	std::vector<size_t> m_chase_times;
	//std::vector<size_t> m_patrol_times;
	std::vector<State> m_states;
	std::vector<Entity> m_bandits;
	std::vector<Entity> m_targets;

	float get_distance(Entity& target, Entity& bandit);

	void handle_idle(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);
	bool can_chase(float& distance_1, float& distance_2, size_t& idle_time);
	bool can_search(Entity target);

	void handle_chase(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);
	bool cannot_chase(float& distance_1, float& distance_2, size_t& chase_time);
	void chase(float& distance_1, float& distance_2, Entity& bandit, float& speed, float& elapsed_ms);
	void follow_direction(Entity& target, Entity& bandit, float& speed, float& elapsed_ms);
	bool is_target_move_toward_bandit(vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir);
	bool is_target_move_away_bandit(vec2& bandit_transform_pos, vec2& target_transform_pos, vec2& target_motion_dir);

	void handle_search(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);

	void handle_patrol(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);

	void check(
		State& state, size_t& idle_time, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& speed, float& elapsed_ms
	);

	// C++ rng
	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist;
	std::random_device rd;

	// Path finding
	const float THRESHOLD_DECIMAL_POINTS = 1e-3f;
	enum struct PathState
	{
		NONE,
		INIT,
		FOUND,
		NOT_FOUND,
		OUT_OF_BOUND
	};

	int OFFSET_ROWS = 6;
	int OFFSET_COLS = 12;
	
	std::shared_ptr<Tilemap> m_tilemap;
	std::vector<Tile> m_path;
	int path_idx = 0;
	Tile m_init_tile;
	Tile m_goal_tile;
	
	std::vector<Tile> init_path_finding(std::shared_ptr<Tilemap> tilemap, Tile init_tile, Tile goal_tile);
	void clear_path_finding();
	std::vector<Tile> do_BFS();
	bool is_next_good(Tile next, Tile curr, std::vector<std::vector<bool>>& visited_matrix);
	std::vector<Tile> assemble_path(std::vector<std::vector<Tile>>& parents_matrix, Tile init_tile, Tile goal_tile);
	bool is_equal(Tile a, Tile b);
	bool is_visited(Tile tile, std::vector<std::vector<bool>>& visited_matrix);
	bool is_within_bandit_region(Tile tile);
	bool is_within_bandit_region(int idx_row, int idx_col, vec2 pos);
	void move_on_path(std::vector<Tile> path, float& speed, float& elapsed_ms, vec2& bandit_pos);
};


#endif //CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
