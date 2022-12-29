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
		GlowOrb(vec2 position, float player_strength, CollidableType object_type, Surface* source_layer);
		void Update(float deltaTime);
		bool IsExpired() { return is_expired; }
		void Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom) override;
		virtual void SetPhase(Phase new_phase) {}

	protected:		
		virtual void UpdateFullPhase(float deltaTime) {}
		virtual void UpdateWaxingPhase(float deltaTime) {}
		virtual void UpdateWaningPhase(float deltaTime) {}
		virtual void UpdateEveryPhase(float deltaTime) {}

		virtual void DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity);

		Phase phase{ Phase::WAXING };
		float opacity{ 1.0f };
		bool is_expired{ false };
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
		void UpdateBounds();		

		Surface* m_source_layer;
	};
};