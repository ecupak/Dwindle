#pragma once

#include <vector>

#include "surface.h"
#include "collidable.h"

namespace Tmpl8
{

	class Obstacle: public Collidable
	{
	public:
		Obstacle(int x, int y, int tile_size, CollidableType object_type, Surface* autotile_map, int autotile_max_frame_id, int autotile_frame_id, int bitwise_overlap, bool is_revealed = false);
		
		virtual void Draw(Surface* screen);
		void Obstacle::ApplyBitwiseOverlap();

		//bool m_is_revealed{ false };

	private:
		Sprite m_sprite;
		int m_autotile_id;
		int m_bitwise_overlap;

		std::vector<CollidableType> m_collidables_of_interest{
			CollidableType::PLAYER_POINT,
		};

	};

};
