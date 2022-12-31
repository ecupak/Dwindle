#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class PlayerGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		PlayerGlowOrb(vec2 position, float player_strength, Surface* source_layer);
		void SetPosition(vec2 position);

	private:
		// METHODS.
		void DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity) override;
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;
	};
};