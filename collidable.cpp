#include "collidable.h"

namespace Tmpl8
{
	int Collidable::id_generator{ 0 };

	Collidable::Collidable() :
		m_id{id_generator++}
	{	}

};