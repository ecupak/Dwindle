#include "safe_glow_orb.h"


namespace Tmpl8
{
	SafeGlowOrb::SafeGlowOrb(vec2 position, float player_strength, Surface* source_layer) :
		GlowOrb{ position, player_strength, CollidableType::SAFE_GLOW, source_layer, 2 }
	{
		SetCollidablesWantedBitflag(m_collidables_of_interest);
		radius_max = 30.0f;
		radius = radius_max;
		phase = Phase::FULL;
		opacity = 235.0f;
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
			destination_pix[x_pos] = MixAlpha(glow_color, new_opacity, 0xFF000000, true);		
		}
	}
};