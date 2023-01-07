#include "collidable.h"

namespace Tmpl8
{
	int Collidable::id_generator{ 0 };
	
	Collidable::Collidable(CollidableInfo collidable_info) :
		m_id{ id_generator++ },
		m_collidable_type{ collidable_info.m_collidable_type },
		m_collision_layer{ GetCollisionId(collidable_info.m_collision_layer) },
		m_collision_mask{ GetCollisionId(collidable_info.m_collision_mask) },
		m_draw_order{ collidable_info.m_draw_order },
		m_center{ collidable_info.m_center }
	{	}


	Collidable::Collidable(CollidableInfo collidable_info, int x, int y, int TILE_SIZE) :
		m_id{ id_generator++ },
		m_collidable_type{ collidable_info.m_collidable_type },
		m_collision_layer{ GetCollisionId(collidable_info.m_collision_layer) },
		m_collision_mask{ GetCollisionId(collidable_info.m_collision_mask) },
		m_draw_order{ collidable_info.m_draw_order },
		m_center{ vec2{ static_cast<float>((x * TILE_SIZE) + (TILE_SIZE / 2.0f)), static_cast<float>((y * TILE_SIZE) + (TILE_SIZE / 2.0f))} },
		left{ x * TILE_SIZE },
		right{ left + TILE_SIZE - 1 },
		top{ y * TILE_SIZE },
		bottom{ top + TILE_SIZE - 1 }
	{	}


	int Collidable::GetCollisionId(std::vector<CollisionType>& collision_types)
	{
		int bitflag{ 0 };
		for (CollisionType& collision_type : collision_types)
		{
			bitflag |= GetCollisionId(collision_type);
		}
		return bitflag;
	}


	int Collidable::GetCollisionId(CollisionType& collision_types)
	{
		switch (collision_types)
		{
		case CollisionType::PLAYER:
			return 0x01;
		case CollisionType::CAMERA:
			return 0x10;
		case CollisionType::BOTH:
			return 0x11;
		default:
			return 0;
		}
	}
};