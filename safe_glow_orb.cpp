#include "safe_glow_orb.h"


namespace Tmpl8
{
	SafeGlowOrb::SafeGlowOrb(vec2 position, float player_strength, Surface* source_layer) :
		GlowOrb{ position, player_strength, CollidableType::SAFE_GLOW, source_layer}
	{
		radius_max = 30.0f;
		radius = radius_max;
		phase = Phase::FULL;
		opacity = 150.0f * player_strength;
	}

	void SafeGlowOrb::DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity)
	{
		/*
			If source pixel has a blue channel value greater than 10, draw a glow at that position.
			Reason is that the solid "black" of the obstacle has an equal alpha to the outline of the obstacle, so that can't be used.
			Additionally, the "blacK" will be made transparent by the template if left as 0xFF000000. So it has a value of 0xFF000001.
			Thus, any pixel on the obstacle that has a blue channel value greater than 1 is the outline of the obstacle.
		*/

		if ((source_pix[x_pos] & BlueMask) > 1)
		{
			destination_pix[x_pos] = MixAlpha(source_pix[x_pos], new_opacity, 0x0000FF00, true);
		}
	}
};