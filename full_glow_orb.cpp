#include "full_glow_orb.h"


namespace Tmpl8
{
	FullGlowOrb::FullGlowOrb(vec2 position, float player_strength, Surface* source_layer, Socket<GlowMessage>* glow_socket, SafeGlowInfo& safe_glow_info) :
		GlowOrb{ CollidableInfo{CollidableType::GLOW_ORB_FULL, CollisionLayer::CAMERA, CollisionMask::NONE, 1, position}, player_strength, source_layer },
		m_glow_socket{ glow_socket }
	{
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
			m_opacity = (radius / radius_max) * 255 * m_player_strength;
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
		m_opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void FullGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		m_opacity = Clamp(m_opacity, 0.0f, 230.0f); // Limited to less than full brightness so not drawn over safe/perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);
	}
};