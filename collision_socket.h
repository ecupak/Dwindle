#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"

namespace Tmpl8
{
	struct CollisionMessage
	{
	public:
		CollisionMessage(std::vector<Collidable*>& collidables)	:
			m_collidables{collidables}
		{	}


		std::vector<Collidable*> m_collidables;
	};
};