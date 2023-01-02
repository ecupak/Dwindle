#include "collidable.h"

namespace Tmpl8
{
	int Collidable::id_generator{ 0 };

	Collidable::Collidable(int x, int y, int tile_size, CollidableType object_type) :
		m_id{ id_generator++ },		
		m_object_type{ object_type },
		left{ x * tile_size },
		right{ left + tile_size - 1 },
		top{ y * tile_size },
		bottom{ top + tile_size - 1 }
	{	}

};