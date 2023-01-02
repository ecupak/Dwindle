#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class PermanentGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		PermanentGlowOrb(vec2 position, float player_strength, float given_radius, Surface* source_layer);

	private:
		// METHODS.
		void DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity) override;
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;

		unsigned int glow_color{ 0xFFFFFFFF };
	};
};