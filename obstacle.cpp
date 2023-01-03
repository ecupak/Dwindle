#include "obstacle.h"

#include <string>


namespace Tmpl8
{
	//TODO:  Move a lot of thinking from the level file to within Obstacle.

	Obstacle::Obstacle(int x, int y, int TILE_SIZE, CollidableType object_type, Surface* autotile_map, int autotile_max_frame_id, int autotile_frame_id, int bitwise_overlap, bool is_revealed) :
		Collidable{x, y, TILE_SIZE, object_type, 0 },
		m_sprite{ autotile_map, autotile_max_frame_id, false },
		m_autotile_id{ autotile_frame_id },
		m_bitwise_overlap{ bitwise_overlap }
		//m_is_revealed{ is_revealed }
	{
		SetCollidablesWantedBitflag(m_collidables_of_interest);

		m_sprite.SetFrame(m_autotile_id);

		center.x = left + m_sprite.GetWidth() / 2.0f;
		center.y = top + m_sprite.GetHeight() / 2.0f;
	}


	void Obstacle::ApplyBitwiseOverlap()
	{
		// Extend bounding box into neighbor by 1 pixel.
		// Solution to player sometimes "slipping" between obstacles.
		if (m_object_type != CollidableType::UNREACHABLE)
		{
			left -= ((m_bitwise_overlap & 0x0001) > 0 ? 1 : 0);
			right += ((m_bitwise_overlap & 0x0010) > 0 ? 1 : 0);
			top -= ((m_bitwise_overlap & 0x0100) > 0 ? 1 : 0);
			bottom += ((m_bitwise_overlap & 0x1000) > 0 ? 1 : 0);
		}
		
	}


	void Obstacle::Draw(Surface* layer)
	{
		m_sprite.Draw(layer, left, top);		
	}
};