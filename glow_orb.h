#pragma once

#include "surface.h"
#include "collidable.h"
#include "glow_socket.h"


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
		GlowOrb(vec2 position, CollidableType object_type);
		void Update();
		bool IsExpired() { return is_expired; }

	protected:		
		virtual void UpdateFullPhase() {}
		virtual void UpdateWaxingPhase() {}
		virtual void UpdateWaningPhase() {}
		virtual void UpdateEveryPhase() {}

		virtual void DrawStep(int x_pos, Pixel*& visible_pix, Pixel*& hidden_pix, int new_opacity, float intensity);

		Phase phase{ Phase::WAXING };
		float opacity{ 1.0f };
		bool is_expired{ false };
		float radius{ 1.0f };

		/* Orb growth. */
		float radius_max{ 140.0f };
		float radius_delta{ 8.0f };
		float opacity_delta{ 0.0f };
		float opacity_delta_delta{ 0.1f };

		/* Orb duration at max. */		
		int delay_max{ 70 };
		int delay{ delay_max };

	private:
		// METHODS.
		void UpdateByPhase();
		void UpdateBounds();
		void Draw(Surface* viewable_screen, Surface* hidden_screen, int v_left, int v_top, int v_right, int v_bottom);
	};
};