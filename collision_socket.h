#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"

namespace Tmpl8
{
	enum class CollisionAction
	{
		UPDATE_ORB_LIST,
		DISABLE_PLAYER_COLLISIONS,
	};


	struct CollisionMessage
	{
	public:
		CollisionMessage(CollisionAction action) :
			m_action{ action }
		{	}

		CollisionMessage(CollisionAction action, std::vector<Collidable*>& collidables) :
			m_action{ action },
			m_collidables{collidables}
		{	}


		CollisionAction m_action;
		std::vector<Collidable*> m_collidables;
	};
};