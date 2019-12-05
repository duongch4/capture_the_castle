//
// Created by Owner on 2019-10-16.
//

#ifndef CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP

#include <memory>
#include <random>
#include <cmath>

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
	void reset() override;

private:
	enum struct State
	{
		IDLE,
		CHASE,
		SEARCH,
		PATROL,
		HOP
	};

	const size_t MAX_BANDITS = BanditSpawnSystem::get_max_bandits();
	const size_t IDLE_LIMIT = 10; // beats
	const size_t CHASE_DISTANCE_THRESHOLD = 80;
	const size_t CHASE_LIMIT = 50; // beats
	const size_t PATROL_LIMIT = 100;
	const float BASE_SPEED = 40.f;
	const float SPEED_UP = BASE_SPEED * 1.2f;

	std::vector<size_t> m_idle_times;
	std::vector<size_t> m_chase_times;
	std::vector<size_t> m_patrol_times;
	std::vector<State> m_states;
	std::vector<Entity> m_bandits;
	std::vector<Entity> m_targets;
	std::shared_ptr<Tilemap> m_tilemap;

	std::vector<vec2> m_prev_dirs;

private:
	void handle_idle(
		State& state, size_t& idle_time, size_t& chase_time,
		const float& distance_1, const float& distance_2, const Entity& bandit
	);

	void handle_patrol(
		State& state, size_t& idle_time, size_t& chase_time,
		const float& distance_1, const float& distance_2,
		const Entity& bandit, vec2& prev_dir
	);

	void handle_chase(
		State& state, size_t& chase_time,
		const float& distance_1, const float& distance_2, const Entity& bandit
	);

	bool can_move(const Tile& tile);
	bool is_within_bandit_region(const Tile& tile);

	float get_distance(const Entity& target, const Entity& bandit) const;

	bool can_chase(const float& distance_1, const float& distance_2, size_t& idle_time);
	bool can_chase_target(const Entity& target, const float& distance);
	void chase(const float& distance_1, const float& distance_2, const Entity& bandit);

	void follow_direction(const Entity& target, const Entity& bandit);
	bool is_target_move_toward_bandit(const vec2& bandit_transform_pos, const vec2& target_transform_pos, const vec2& target_motion_dir);
	bool is_target_move_away_bandit(const vec2& bandit_transform_pos, const vec2& target_transform_pos, const vec2& target_motion_dir);

private:
	const float HOP_DELAY = 0.5f;
	float m_hop_timer = 0.f;

	void handle_idle_search(
		State& state, size_t& chase_time,
		const float& distance_1, const float& distance_2, const Entity& bandit
	);
	bool can_search(Entity target);
	void handle_search(State& state, Entity& bandit);
	void handle_hop(
		State& state, size_t& chase_time,
		float& distance_1, float& distance_2,
		Entity& bandit, float& elapsed_ms
	);

private:
	// C++ rng
	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist;
	std::random_device rd;

private:
	// Path finding
	std::vector<Tile> m_path;
	int path_idx = 0;
	Tile m_init_tile;
	Tile m_goal_tile;
	
private:
	std::vector<Tile> init_path_finding(std::shared_ptr<Tilemap> tilemap, Tile init_tile, Tile goal_tile);
	void clear_path_finding();
	std::vector<Tile> do_BFS();
	bool is_next_good(Tile next, Tile curr, std::vector<std::vector<bool>>& visited_matrix);
	std::vector<Tile> assemble_path(std::vector<std::vector<Tile>>& parents_matrix, Tile init_tile, Tile goal_tile);
	bool is_equal(Tile a, Tile b);
	bool is_visited(Tile tile, std::vector<std::vector<bool>>& visited_matrix);
};


#endif //CAPTURE_THE_CASTLE_BANDIT_AI_SYSTEM_HPP
