#include "player_glow_orb.h"


namespace Tmpl8
{
	PlayerGlowOrb::PlayerGlowOrb(vec2 position, float player_strength, Surface* source_layer) :
		GlowOrb{ position, player_strength, CollidableType::PLAYER_GLOW, source_layer }
	{
		radius_max = 180.0f;
		radius = radius_max;
		phase = Phase::FULL;
		opacity = 220.0f;
	}

	
	void PlayerGlowOrb::SetPosition(vec2 position)
	{
		center = position;
		UpdateBounds();
	}


	void PlayerGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void PlayerGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 255.0f);
		radius = Clamp(radius, 0.0f, radius_max);
	}


	void PlayerGlowOrb::DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity)
	{
		/*
			Draw anything with an alpha value that overlaps the orb on the text_layer.
		*/

		new_opacity *= intensity;

		if ((source_pix[x_pos] >> 24) > 1)
		{
			destination_pix[x_pos] = MixAlpha(source_pix[x_pos], new_opacity, 0xFF000000, true);
		}
	}
};