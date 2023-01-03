#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"

namespace Tmpl8
{
	enum class CollisionAction
	{
		UPDATE_ORB_LIST,
		UPDATE_PLAYER_POINT_LIST,
		UPDATE_OBSTACLE_LIST,
		UPDATE_PICKUP_LIST,
		UPDATE_FINISH_LINE_LIST,
		UPDATE_UNIQUE_LIST,
		DISABLE_PLAYER_COLLISIONS,
	};


	struct CollisionMessage
	{
	public:
		CollisionMessage(CollisionAction action) :
			m_action{ action }
		{	}


		CollisionMessage(CollisionAction action, Collidable* collidable) :
			m_action{ action },
			m_collidable{ collidable }
		{	}


		CollisionMessage(CollisionAction action, std::vector<Collidable*>& collidables) :
			m_action{ action },
			m_collidables{collidables}
		{	}


		CollisionAction m_action;
		Collidable* m_collidable{ nullptr };
		std::vector<Collidable*> m_collidables;
	};
};