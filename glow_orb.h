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
		GlowOrb(vec2 position, bool is_from_ricochet);
		void Update();

		bool IsExpired() { return is_expired; }

		virtual void Draw(Surface* viewable_screen, Surface* hidden_screen, int v_left, int v_top, int v_right, int v_bottom);

		// ATTRIBUTES.
		bool is_expired{ false };
		float radius{ 0.0f };

	private:
		// METHODS.
		void UpdateSizeAndOpacity();
		void UpdateBounds();

		// ATTRIBUTES.
		Phase phase{ Phase::WAXING };
		float opacity{ 1.0f };

		/* Orb growth. */
		float radius_max{ 140.0f };
		float radius_delta{ 8.0f };
		float opacity_delta{ 0.0f };
		float opacity_delta_delta{ 0.1f };

		/* Orb duration at max. */
		int delay{ 0 };
		int delay_max{ 70 };
	};
};