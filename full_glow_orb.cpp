#include "full_glow_orb.h"


namespace Tmpl8
{
	FullGlowOrb::FullGlowOrb(vec2 position, GlowSocket* glow_socket) :
		GlowOrb{ position, CollidableType::FULL_GLOW },
		m_glow_socket{ glow_socket }
	{
		radius_max = 240.0f;
		delay_max = 40.0f;
		delay = delay_max;
	}


	void FullGlowOrb::UpdateWaxingPhase()
	{
		if (radius >= radius_max)
		{
			phase = Phase::FULL;
			m_glow_socket->SendMessage(center, CollidableType::SAFE_GLOW); // Spawn a safe glow orb at this position.		
		}
		else
		{
			radius += radius_delta;
			opacity = (radius / radius_max) * 255;
		}
	}
	
	
	void FullGlowOrb::UpdateFullPhase()
	{
		if (--delay <= 0)
		{
			phase = Phase::WANING;
		}
	}

	
	void FullGlowOrb::UpdateWaningPhase()
	{
		opacity -= opacity_delta;
		opacity_delta += opacity_delta_delta;
	}


	void FullGlowOrb::UpdateEveryPhase()
	{
		opacity = Clamp(opacity, 0.0f, 230.0f);
		radius = Clamp(radius, 0.0f, radius_max);
	}
};