#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"


namespace Tmpl8
{
	enum class GlowAction
	{	
		LEVEL_RESET,
		MAKE_ORB,
		MOVE_PLAYER_ORB_POSITION,
	};


	struct GlowMessage
	{
	public:
		// level reset
		GlowMessage(GlowAction action) :
			m_action{ action }
		{	}

		// move player orb
		GlowMessage(GlowAction action, vec2& orb_position) :
			m_action{ action },
			m_orb_position{ orb_position }
		{	}

		// make orb
		GlowMessage(GlowAction action, vec2& orb_position, float player_strength, CollidableType glow_orb_type, bool is_safe_glow_needed = false) :
			m_action{ action },
			m_orb_position{ orb_position },
			m_player_strength{ player_strength },
			m_glow_orb_type{ glow_orb_type },
			m_is_safe_glow_needed{ is_safe_glow_needed }
		{	}


		GlowAction m_action;
		vec2 m_orb_position{ 0.0f, 0.0f };
		float m_player_strength{ 0.0f };
		CollidableType m_glow_orb_type{ CollidableType::UNKNOWN };
		bool m_is_safe_glow_needed{ false };
	};
};