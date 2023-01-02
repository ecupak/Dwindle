#pragma once

#include "socket.h"


namespace Tmpl8
{
	enum class GameAction
	{
		PLAYER_DEATH,
		ORBS_REMOVED,
		PLAYER_IN_FREE_FALL,
		PLAYER_SUSPENDED,
		LEVEL_COMPLETE,
	};


	struct GameMessage
	{
	public:
		GameMessage(GameAction action) :
			m_action{ action }
		{	}


		GameAction m_action;		
	};
};