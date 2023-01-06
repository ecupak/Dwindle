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
		Obstacle(int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite);

		void Draw(Surface* screen) override;
		void ApplyOverlap();

	private:		
		CollidableType GetCollidableType(int tile_id);		
		int GetFrameId(int autotile_id);

		Sprite& m_sprite;
		int m_frame_id{ 0 };
		int m_autotile_id{ 0 };

		std::vector<CollidableType> m_collidables_of_interest{
			CollidableType::PLAYER_POINT,
		};
	};

};
