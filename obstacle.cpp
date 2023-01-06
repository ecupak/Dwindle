#include "obstacle.h"

#include <string>

#include "blueprint_code.h"


namespace Tmpl8
{
	Obstacle::Obstacle(int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite) :
		Collidable{ x, y, TILE_SIZE, GetCollidableType(tile_id), 0 },
		m_sprite{ sprite },
		m_frame_id{ GetFrameId(autotile_id) },
		m_autotile_id{ autotile_id }
	{	
		SetCollidablesWantedBitflag(m_collidables_of_interest);	
	}

	
	void Obstacle::Draw(Surface* layer)
	{
		m_sprite.SetFrame(m_frame_id);
		m_sprite.Draw(layer, left, top);
	}


	void Obstacle::ApplyOverlap()
	{
		// Extend bounding box into neighbor by 1 pixel.
		// Solution to detector points sometimes "slipping" between obstacles.
		if (m_object_type != CollidableType::OBSTACLE_UNREACHABLE)
		{
			if (m_autotile_id & LEFT)
				--left;

			if (m_autotile_id & RIGHT)
				++right;

			if (m_autotile_id & TOP)
				--top;

			if (m_autotile_id & BOTTOM)
				++bottom;
		}
	}


	CollidableType Obstacle::GetCollidableType(int tile_id)
	{		
		switch (tile_id)
		{
		case VISIBLE_OBSTACLE_TILE:
			return CollidableType::OBSTACLE_VISIBLE;
		case HIDDEN_OBSTACLE_TILE:
			return CollidableType::OBSTACLE_HIDDEN;
		case UNREACHABLE_OBSTACLE_TILE:
			return CollidableType::OBSTACLE_UNREACHABLE;
		case DANGEROUS_OBSTACLE_TILE:
			return CollidableType::OBSTACLE_DANGEROUS;
		}		
	}


	int Obstacle::GetFrameId(int autotile_id)
	{
		switch (autotile_id)
		{
		case 208:	return 0;
		case 248:	return 1;
		case 104:	return 2;
		case 64:	return 3;
		case 80:	return 4;
		case 216:	return 5;
		case 120:	return 6;
		case 72:	return 7;
		case 214:	return 8;
		case 255:	return 9;
		case 107:	return 10;
		case 66:	return 11;
		case 210:	return 12;
		case 254:	return 13;
		case 251:	return 14;
		case 106:	return 15;
		case 22:	return 16;
		case 31:	return 17;
		case 11:	return 18;
		case 2:		return 19;
		case 86:	return 20;
		case 223:	return 21;
		case 127:	return 22;
		case 75:	return 23;
		case 16:	return 24;
		case 24:	return 25;
		case 8:		return 26;
		case 0:		return 27; // default return;
		case 18:	return 28;
		case 30:	return 29;
		case 27:	return 30;
		case 10:	return 31;
		case 88:	return 32;
		case 74:	return 33;
		case 250:	return 34;
		case 123:	return 35;
		case 218:	return 36;
		case 122:	return 37;
		case 126:	return 38;
		case 219:	return 39;
		case 82:	return 40;
		case 26:	return 41;
		case 222:	return 42;
		case 95:	return 43;
		case 94:	return 44;
		case 91:	return 45;
		case 90:	return 46;
		default:	return 27; // isolated wall (case 0).
		}
	}
};