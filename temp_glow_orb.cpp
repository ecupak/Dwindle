#include "temp_glow_orb.h"


namespace Tmpl8
{
	TempGlowOrb::TempGlowOrb(vec2 position, float player_strength, Surface* source_layer) :
		GlowOrb{ CollidableInfo{CollidableType::GLOW_ORB_TEMP, CollisionLayer::CAMERA, CollisionMask::NONE, 1, position}, player_strength, source_layer }
	{
		radius_max = 100.0f;
		delay = delay_max;
	}


	void TempGlowOrb::UpdateWaxingPhase(float deltaTime)
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


	void TempGlowOrb::UpdateFullPhase(float deltaTime)
	{
		delay -= deltaTime;

		if (delay <= 0)
		{
			phase = Phase::WANING;
		}
	}


	void TempGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		m_opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void TempGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		m_opacity = Clamp(m_opacity, 0.0f, 230.0f); // Limited to less than full brightness so not drawn over safe/perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);
	}
};