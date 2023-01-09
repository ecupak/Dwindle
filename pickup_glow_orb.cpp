#include "pickup_glow_orb.h"

namespace Tmpl8
{
	PickupGlowOrb::PickupGlowOrb(vec2 position, float player_strength, Surface* source_layer, int parent_id) :
		GlowOrb{ CollidableInfo{CollidableType::GLOW_ORB_PICKUP, CollisionLayer::CAMERA, CollisionMask::NONE, 1, position}, player_strength, source_layer }
	{
		m_parent_id = parent_id;

		radius_max = 110.0f;
		radius = radius_max;
		phase = Phase::FULL;
		m_opacity = 230.0f;
	}


	void PickupGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		m_opacity -= opacity_delta * deltaTime;
		opacity_delta += opacity_delta_delta * deltaTime;
	}


	void PickupGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		m_opacity = Clamp(m_opacity, 0.0f, 230.0f); // Limited to less than full brightness so not drawn over perm orbs.
		radius = Clamp(radius, 0.0f, radius_max);
	}
};
