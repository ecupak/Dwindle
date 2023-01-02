#include "permanent_glow_orb.h"

namespace Tmpl8
{
	PermanentGlowOrb::PermanentGlowOrb(vec2 position, float player_strength, float given_radius, Surface* source_layer) :
		GlowOrb{ position, player_strength, CollidableType::SAFE_GLOW, source_layer }
	{
		radius_max = given_radius;
		radius = radius_max;
		phase = Phase::FULL;
		opacity = 240.0f;
	}


	void PermanentGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void PermanentGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 240.0f);  // Drawn over all other orbs.
		radius = Clamp(radius, 0.0f, radius_max);
	}


	void PermanentGlowOrb::DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity)
	{
		/*
			If source pixel has a blue channel value greater than 1, draw a glow at that position.
			Can't compare alpha values (outline color and fill color of obstacle are both 0xFF......).
			Can't look for a zero RGB value (the "blacK" will be made transparent by the template, so it has a value of 0xFF000001).
			Thus, any pixel on the obstacle that has a blue channel value greater than 1 is the outline of the obstacle.
		*/

		/*if ((source_pix[x_pos] & BlueMask) > 1)
		{		
			destination_pix[x_pos] = MixAlpha(glow_color, new_opacity, 0xFF000000, true);		
		}*/
	}
};