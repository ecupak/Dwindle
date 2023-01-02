#pragma once

#include "socket.h"
#include "template.h"

namespace Tmpl8
{
	enum class LifeAction
	{
		UPDATE,
		TUTORIAL_SAVE,
	};

	struct LifeMessage
	{
	public:
		LifeMessage(LifeAction action, int new_value, float player_strength) :
			m_action{ action },
			m_new_value{ new_value },
			m_player_strength{ player_strength }
		{	}

		LifeAction m_action;
		int m_new_value;
		float m_player_strength;
	};
};