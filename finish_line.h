#pragma once

#include "collidable.h"


namespace Tmpl8
{
	class FinishLine : public Collidable
	{
	public:
		FinishLine(int x, int y, int TILE_SIZE, CollidableType object_type) :
			Collidable{ x, y, TILE_SIZE, object_type, 0 }
		{	
			SetCollidablesWantedBitflag(m_collidables_of_interest);
		}

	private:
		std::vector<CollidableType> m_collidables_of_interest{			
			CollidableType::PLAYER,
		};
	};
};

