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
		GlowOrb(CollidableInfo collidable_info, float player_strength, Surface* source_layer);
		
		void Update(float deltaTime);
		void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity = 1.0f) override;
		virtual void SetPhase(Phase new_phase);

		int m_parent_id{ 0 };

	protected:
		virtual void UpdateFullPhase(float deltaTime) {}
		virtual void UpdateWaxingPhase(float deltaTime) {}
		virtual void UpdateWaningPhase(float deltaTime) {}
		virtual void UpdateEveryPhase(float deltaTime) {}

		void UpdateBounds();
		
		virtual void DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity);

		Phase phase{ Phase::WAXING };
		float m_opacity{ 1.0f };
		float radius{ 1.0f };
		
		float m_player_strength{ 1.0f };

		/* Orb growth. */
		float radius_max{ 140.0f };
		float radius_delta{ 1000.0f };

		float opacity_delta{ 0.0f };
		float opacity_delta_delta{ 200.0f };

		/* Orb duration at max. */		
		float delay_max{ 0.0f };
		float delay{ delay_max };

	private:
		// METHODS.
		void UpdateByPhase(float deltaTime);
		
		Surface* m_source_layer;
	};
};