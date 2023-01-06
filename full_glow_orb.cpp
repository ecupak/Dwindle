#include "full_glow_orb.h"


namespace Tmpl8
{
	FullGlowOrb::FullGlowOrb(vec2 position, float player_strength, Surface* source_layer, Socket<GlowMessage>* glow_socket, bool is_safe_glow_needed, bool is_on_dangerous_tile) :
		GlowOrb{ position, player_strength, CollidableType::FULL_GLOW, source_layer, 1 },
		m_glow_socket{ glow_socket }
	{
		SetCollidablesWantedBitflag(m_collidables_of_interest);

		radius_max = 180.0f;
		delay_max = 0.25f;
		delay = delay_max;

		if (is_safe_glow_needed)
		{
			m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, position, player_strength, CollidableType::SAFE_GLOW, (is_on_dangerous_tile ? 0.28f : -1.0f) });
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
			radius += radius_delta * deltaTime;
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
		opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void FullGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 230.0f); // Limited to less than full brightness so not drawn over safe/perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);
	}
};