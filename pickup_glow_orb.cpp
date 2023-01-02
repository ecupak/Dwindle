#include "pickup_glow_orb.h"

namespace Tmpl8
{
	PickupGlowOrb::PickupGlowOrb(vec2 position, float player_strength, Surface* source_layer) :
		GlowOrb{ position, player_strength, CollidableType::PICKUP_GLOW, source_layer }
	{
		radius_max = 180.0f;
		radius = radius_max;
		phase = Phase::FULL;
		opacity = 230.0f;
	}


	void PickupGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void PickupGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 230.0f); // Limited to less than full brightness so not drawn over perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);
	}
};