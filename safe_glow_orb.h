#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class SafeGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		SafeGlowOrb(vec2 position, Surface* source_layer);

	private:
		// METHODS.
		void DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity) override;
	};
};