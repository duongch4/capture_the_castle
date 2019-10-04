#pragma once
#include "common.hpp"
#include "player.hpp"
class ItemBoard :public Entity
{
public:
	ItemBoard(Team team, vec2 position);
	~ItemBoard();

	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();

	// Update player position based on direction
	void update(float ms);

	// Renders the player
	void draw(const mat3& projection)override;

	void update_item(Texture it);

	const Team get_team();

private:
	Texture item_texture;

	Texture board_texture;

	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;

	bool is_board_texture_loaded(const char* path);
	bool load_item_texture(const char* path);
};

