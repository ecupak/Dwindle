#pragma once

#include <vector>

#include "surface.h"
#include "collidable.h"
#include "obstacle_consts.h"

namespace Tmpl8
{

	class Obstacle: public Collidable
	{
	public:
		//Obstacle(int x, int y, int tile_size, CollidableType object_type, Surface* autotile_map, int autotile_max_frame_id, int autotile_frame_id, int bitwise_overlap, bool is_revealed = false);
		
		Obstacle(int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite);

		void Draw(Surface* screen) override;
		void ApplyOverlap();

		//bool m_is_revealed{ false };

	private:		
		CollidableType GetCollidableType(int autotile_id, int tile_id);		
		int GetFrameId(int autotile_id);

		Sprite& m_sprite;
		int m_frame_id{ 0 };
		int m_autotile_id{ 0 };

		//int m_autotile_id;
		//int m_bitwise_overlap;

		std::vector<CollidableType> m_collidables_of_interest{
			CollidableType::PLAYER_POINT,
		};

	};

};
