#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class SafeGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		SafeGlowOrb(vec2 position, float player_strength, Surface* source_layer, SafeGlowInfo safe_glow_info);

	private:
		// METHODS.
		void DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity) override;
		void UpdateFullPhase(float deltaTime) override;
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;

		unsigned int glow_color{ 0xFF00FF00 };
		bool m_is_on_delay{ false };
		Collidable* m_moving_obstacle{ nullptr };
	};
};