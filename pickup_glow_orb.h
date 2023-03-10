#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class PickupGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		PickupGlowOrb(vec2 position, float player_strength, Surface* source_layer, int parent_id);

	private:
		// METHODS.
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;
	};
};
