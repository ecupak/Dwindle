#pragma once

#include "collidable.h"


namespace Tmpl8
{
	class FinishLine : public Collidable
	{
	public:
		FinishLine(int x, int y, int TILE_SIZE, CollidableType collidable_type) :
			Collidable{ CollidableInfo{collidable_type, CollisionLayer::PLAYER, CollisionMask::NONE, 0}, x, y, TILE_SIZE }
		{	}	
	};
};

