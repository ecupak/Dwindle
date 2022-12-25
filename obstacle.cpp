#include "obstacle.h"
#include <string>

namespace Tmpl8
{
	Obstacle::Obstacle(int x, int y, int tile_size, CollidableType object_type, Surface* autotile_map, int autotile_max_frame_id, int autotile_frame_id, int bitwise_overlap) :
		m_sprite{ autotile_map, autotile_max_frame_id, false },
		m_autotile_id{ autotile_frame_id }
	{
		m_sprite.SetFrame(m_autotile_id);
		
		m_object_type = object_type;

		left = x * tile_size;
		right = left + tile_size - 1;
		top = y * tile_size;
		bottom = top + tile_size - 1;

		center.x = left + m_sprite.GetWidth() / 2;
		center.y = top + m_sprite.GetHeight() / 2;

		// Extend bounding box into neighbor by 1 pixel.
		// Solution to player sometimes "slipping" between obstacles.
		if (m_object_type != CollidableType::UNREACHABLE)
		{
			left -= ((bitwise_overlap & 0x0001) > 0 ? 1 : 0);
			right += ((bitwise_overlap & 0x0010) > 0 ? 1 : 0);
			top -= ((bitwise_overlap & 0x0100) > 0 ? 1 : 0);
			bottom += ((bitwise_overlap & 0x1000) > 0 ? 1 : 0);
		}
		
	}


	void Obstacle::ResolveCollision(Collidable*& collision)
	{
		//m_autotile_id++;
	}


	void Obstacle::ResolveCollision(std::vector<Collidable*> collisions)
	{
		//m_autotile_id += collisions.size();
	}


	void Obstacle::Draw(Surface* screen)
	{
		m_sprite.Draw(screen, left, top);		
	}
};