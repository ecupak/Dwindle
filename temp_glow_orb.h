#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class TempGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		TempGlowOrb(vec2 position, float player_strength, Surface* source_layer);

	private:
		// METHODS.
		void UpdateFullPhase(float deltaTime) override;
		void UpdateWaxingPhase(float deltaTime) override;
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;
	};
};