#include "safe_glow_orb.h"


namespace Tmpl8
{
	SafeGlowOrb::SafeGlowOrb(vec2 position, float player_strength, Surface* source_layer, SafeGlowInfo safe_glow_info) :
		GlowOrb{ CollidableInfo{CollidableType::GLOW_ORB_SAFE, CollisionLayer::BOTH, CollisionMask::NONE, 2, position}, player_strength, source_layer },
		m_is_on_delay{ safe_glow_info.m_is_on_delay },
		m_moving_obstacle{ safe_glow_info.m_moving_obstacle }
	{
		phase = Phase::FULL;		
		delay = 0.25f;

		radius_max = 30.0f;
		radius = radius_max;		
		m_opacity = 235.0f;
	}

	
	void SafeGlowOrb::UpdateFullPhase(float deltaTime)
	{
		if (m_is_on_delay)
		{
			delay -= deltaTime;

			if (delay <= 0)
			{
				phase = Phase::WANING;
			}
		}
	}


	void SafeGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		m_opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}

	
	void SafeGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		m_opacity = Clamp(m_opacity, 0.0f, 235.0f); // Limited to less than full brightness so not drawn over perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);

		if (m_moving_obstacle != nullptr)
		{
			m_center += m_moving_obstacle->m_delta_position;
		}
	}


	void SafeGlowOrb::DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity)
	{
		/*
			If source pixel has a blue channel value greater than 1, draw a glow at that position.
			Can't compare alpha values (outline color and fill color of obstacle are both 0xFF......).
			Can't look for a zero RGB value (the "blacK" will be made transparent by the template, so it has a value of 0xFF000001).
			Thus, any pixel on the obstacle that has a blue channel value greater than 1 is the outline of the obstacle.
		*/
				
		if ((source_pix[x_pos] & BlueMask) > 1)
		{
			destination_pix[x_pos] = MixAlpha(glow_color, new_opacity, destination_pix[x_pos], true);
		}
	}
};