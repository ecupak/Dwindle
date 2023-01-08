#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"


namespace Tmpl8
{
	enum class PlayerAction
	{
		DELTA_POSITION,
	};


	struct PlayerMessage
	{
	public:
		PlayerMessage(PlayerAction action, vec2& delta_position) :
			m_action{ action },
			m_delta_position{ delta_position }
		{	}

		PlayerAction m_action;
		vec2 m_delta_position{ 0.0f, 0.0f };
	};
};