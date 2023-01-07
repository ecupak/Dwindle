#include "full_glow_orb.h"


namespace Tmpl8
{
	FullGlowOrb::FullGlowOrb(vec2 position, float player_strength, Surface* source_layer, Socket<GlowMessage>* glow_socket, SafeGlowInfo& safe_glow_info) :
		GlowOrb{ position, player_strength, CollidableType::GLOW_ORB_FULL, source_layer, 1 },
		m_glow_socket{ glow_socket }
	{
		SetCollidablesWantedBitflag(m_collidables_of_interest);

		radius_max = 180.0f;
		delay_max = 0.25f;
		delay = delay_max;

		if (safe_glow_info.m_is_safe_glow_needed)
		{
			m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, position, player_strength, CollidableType::GLOW_ORB_SAFE, safe_glow_info });
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