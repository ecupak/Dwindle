#include "collidable.h"

namespace Tmpl8
{
	int Collidable::id_generator{ 0 };
	
	Collidable::Collidable(CollidableType object_type, int draw_order, vec2 _center) :
		m_id{ id_generator++ },
		m_object_type{ object_type },
		m_collision_id{ GetCollisionId(object_type) },
		m_draw_order{ draw_order },
		center{ _center }
	{	}


	Collidable::Collidable(int x, int y, int tile_size, CollidableType object_type, int draw_order, vec2 _center) :
		m_id{ id_generator++ },
		m_object_type{ object_type },
		m_collision_id{ GetCollisionId(object_type) },
		m_draw_order{ draw_order },
		center{ _center },
		left{ x * tile_size },
		right{ left + tile_size - 1 },
		top{ y * tile_size },
		bottom{ top + tile_size - 1 }
	{	}

	
	void Collidable::SetCollidablesWantedBitflag(std::vector<CollidableType>& collidables_of_interest)
	{
		m_collision_ids_wanted = GetCollisionId(collidables_of_interest);
	}


	int Collidable::GetCollisionId(std::vector<CollidableType>& collidables)
	{
		int bitflag{ 0 };
		for (CollidableType& collidable : collidables)
		{
			bitflag |= GetCollisionId(collidable);
		}
		return bitflag;
	}


	int Collidable::GetCollisionId(CollidableType& collidable)
	{
		switch (collidable)
		{
		case CollidableType::PLAYER_POINT:
			return 0x1;
		case CollidableType::OBSTACLE:
		case CollidableType::PERM_GLOW:
			return 0x10;
		case CollidableType::FINISH_LINE:
			return 0x100;
		case CollidableType::PICKUP:
			return 0x1000;
		case CollidableType::FULL_GLOW:
		case CollidableType::TEMP_GLOW:		
			return 0x10000;
		case CollidableType::SAFE_GLOW:		
		case CollidableType::PICKUP_GLOW:
			return 0x100000;
		case CollidableType::CAMERA:
			return 0x1000000;
		case CollidableType::PLAYER:
			return 0x10000000;
		default:
			return 0;
		}
	}

};