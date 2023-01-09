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
		PLAYER_AT_MAX_FALL,
		LEVEL_COMPLETE,
		START_TUTORIAL,
		START_GAME,
		TUTORIAL_COMPLETE,
		NONE,
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