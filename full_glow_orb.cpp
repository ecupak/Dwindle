#include "full_glow_orb.h"


namespace Tmpl8
{
	FullGlowOrb::FullGlowOrb(vec2 position, float player_strength, Surface* source_layer, Socket<GlowMessage>* glow_socket, bool is_safe_glow_needed) :
		GlowOrb{ position, player_strength, CollidableType::FULL_GLOW, source_layer },
		m_glow_socket{ glow_socket }
	{
		radius_max = 240.0f;
		delay_max = 0.25f;
		delay = delay_max;

		if (is_safe_glow_needed)
		{
			m_glow_socket->SendMessage(GlowMessage{ center, player_strength, CollidableType::SAFE_GLOW });
		}
	}


	void FullGlowOrb::UpdateWaxingPhase(float deltaTime)
	{
		if (radius >= radius_max)
		{
			phase = Phase::FULL;					
		}
		else
		{
			radius += radius_delta;
			opacity = (radius / radius_max) * 255 * m_player_strength;
		}
	}
	
	
	void FullGlowOrb::UpdateFullPhase(float deltaTime)
	{
		delay -= deltaTime;

		if (delay <= 0)
		{
			phase = Phase::WANING;
		}
	}

	
	void FullGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		opacity -= opacity_delta;
		opacity_delta += opacity_delta_delta;
	}


	void FullGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 230.0f);
		radius = Clamp(radius, 0.0f, radius_max);
	}
};