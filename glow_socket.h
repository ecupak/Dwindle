#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"

namespace Tmpl8
{
	struct GlowMessage
	{
	public:
		GlowMessage(vec2& orb_position, CollidableType glow_orb_type) :
			m_orb_position{ orb_position },
			m_glow_orb_type{ glow_orb_type }
		{	}


		vec2 m_orb_position{ 0.0f, 0.0f };
		CollidableType m_glow_orb_type{ CollidableType::UNKNOWN };
	};
};