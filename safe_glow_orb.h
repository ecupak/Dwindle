#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class SafeGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		SafeGlowOrb(vec2 position);

	private:
		// METHODS.
		void DrawStep(int x_pos, Pixel*& visible_pix, Pixel*& hidden_pix, int new_opacity, float intensity) override;
	};
};