#pragma once

#include "socket.h"
#include "template.h"

namespace Tmpl8
{
	struct LifeMessage
	{
	public:
		LifeMessage(int new_value, float player_strength) :
			m_new_value{ new_value },
			m_player_strength{ player_strength }
		{	}

		int m_new_value;
		float m_player_strength;
	};
};