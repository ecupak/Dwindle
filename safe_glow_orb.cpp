#include "safe_glow_orb.h"


namespace Tmpl8
{
	SafeGlowOrb::SafeGlowOrb(vec2 position) :
		GlowOrb{ position, CollidableType::SAFE_GLOW }
	{
		radius_max = 25.0f;
		radius = radius_max;
		phase = Phase::FULL;
		opacity = 150.0f;
	}

	void SafeGlowOrb::DrawStep(int x_pos, Pixel*& visible_pix, Pixel*& hidden_pix, int new_opacity, float intensity)
	{
		/*
			If pixel on the hidden screen (only obstacles) has any alpha, draw it.
			The background has 00 alpha, so this will only draw the obstacle itself.
		*/

		if ((hidden_pix[x_pos] & 15) > 1)
		{
			visible_pix[x_pos] = MixAlpha(hidden_pix[x_pos], new_opacity, 0x0000FFFF, true);
		}
	}
};