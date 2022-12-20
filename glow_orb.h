#pragma once

#include "surface.h"
#include "collidable.h"

namespace Tmpl8
{
	// Track state.
	enum class Phase
	{
		WAXING,
		FULL,
		WANING
	};

	// Class definition.
	class GlowOrb : public Collidable
	{
	public:
		// METHODS.		
		GlowOrb(vec2 position);
		bool Update();

		bool IsExpired() { return is_expired; }
		// ATTRIBUTES.

		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
		bool is_expired{ false };
		float radius{ 0.0f };

	private:
		// METHODS.
		void UpdateSizeAndOpacity();
		void UpdateBounds(Surface* m_glow_orb_layer);
		void DrawGlowOrb(Surface* m_glow_orb_layer);

		// ATTRIBUTES.
		vec2 center{ 0.0f, 0.0f };
		Phase phase{ Phase::WAXING };
		float opacity{ 1.0f };

		/* Orb growth. */
		float radius_max{ 120.0f };
		float radius_delta{ 8.0f };
		float opacity_delta{ 0.0f };
		float opacity_delta_delta{ 0.1f };

		/* Orb duration at max. */
		int delay{ 0 };
		int delay_max{ 60 };
	};
};