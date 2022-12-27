#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"

namespace Tmpl8
{
	struct GlowMessage
	{
	public:
		GlowMessage(vec2& orb_position, CollidableType glow_orb_type, bool is_safe_glow_needed = false) :
			m_orb_position{ orb_position },
			m_glow_orb_type{ glow_orb_type },
			m_is_safe_glow_needed{ is_safe_glow_needed }
		{	}


		vec2 m_orb_position{ 0.0f, 0.0f };
		CollidableType m_glow_orb_type{ CollidableType::UNKNOWN };
		bool m_is_safe_glow_needed;
	};
};