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
		Obstacle(CollidableInfo collidable_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite);
				
		void Draw(Surface* screen) override;
		void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom) override;
		void ApplyOverlap();

	protected:
		Sprite& m_sprite;
		int m_frame_id{ 0 };

	private:
		CollidableType GetCollidableType(int tile_id);		
		int GetFrameId(int autotile_id);
		
		int m_autotile_id{ 0 };
	};

};
