#include "safe_glow_orb.h"


namespace Tmpl8
{
	SafeGlowOrb::SafeGlowOrb(vec2 position, float player_strength, Surface* source_layer, SafeGlowInfo safe_glow_info) :
		GlowOrb{ position, player_strength, CollidableType::GLOW_ORB_SAFE, source_layer, 2 },
		m_is_on_delay{ safe_glow_info.m_is_on_delay }
	{
		SetCollidablesWantedBitflag(m_collidables_of_interest);

		phase = Phase::FULL;		
		delay = 1.0f;

		radius_max = 30.0f;
		radius = radius_max;		
		opacity = 235.0f;
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
		opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}

	
	void SafeGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 235.0f); // Limited to less than full brightness so not drawn over perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);
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