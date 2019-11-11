//
// Created by Owner on 2019-10-12.
//

#ifndef CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
#define CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP

#include <ecs/ecs_manager.hpp>
#include <ecs/events.hpp>
#include <components.hpp>
#include <texture_manager.hpp>
#include <tilemap.hpp>

extern ECSManager ecsManager;

class PlayerInputSystem : public System {
public:
    void init(std::shared_ptr<Tilemap> tilemap);
    void update();

private:
    const std::vector<InputKeys> PLAYER1KEYS = {
		InputKeys::W, InputKeys::A, InputKeys::S, InputKeys::D,
		InputKeys::Q
	};

    const std::vector<InputKeys> PLAYER2KEYS = {
		InputKeys::UP, InputKeys::DOWN, InputKeys::RIGHT, InputKeys::LEFT,
		InputKeys::SLASH
	};
    
	std::map<InputKeys, bool> keysPressed;

	std::shared_ptr<Tilemap> m_tilemap;

private:
    void onKeyListener(InputKeyEvent* input);
    void onReleaseListener(KeyReleaseEvent* input);

private:
	size_t soldier_count_1 = 0;
	size_t soldier_count_2 = 0;
	static const size_t MAX_SOLDIERS = 2;

	size_t wait_1 = 0;
	size_t wait_2 = 0;
	static const size_t RESPAWN_IDLE = 200;

private:
	bool can_spawn(
		const size_t& soldier_count, const size_t& wait_time, const Transform& transform,
		const std::pair<int, int>& tile_idx, const MazeRegion& maze_region
	);
	void spawn_soldier(
		const Transform& transform, const Motion& motion,
		const TeamType& team_type, const char* texture_path
	);
};


#endif //CAPTURE_THE_CASTLE_PLAYER_INPUT_SYSTEM_HPP
