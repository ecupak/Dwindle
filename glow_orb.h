#pragma once

#include "surface.h"
#include "collidable.h"

#include "glow_socket.h"


namespace Tmpl8
{
	/// <summary>
	/// Glow orbs are growing in size (waxing), at maximum size (full), or decreasing in size (waning).	
	/// </summary>
	enum class Phase
	{
		WAXING,
		FULL,
		WANING
	};


	/// <summary>
	/// Glow orbs are the "light" that the player creates by bouncing. Each glow orb is assigned a specific Surface that is created and stored by the level.
	/// Glow orbs copy pixels form their assigned Surface to the screen Surface to give the illusion of illumination.
	/// </summary>
	class GlowOrb : public Collidable
	{
	public:		
		/// <summary>
		/// Constructor.
		/// </summary>
		/// <param name="collidable_info">Data package for Collidable constructor.</param>
		/// <param name="player_strength">Player strength (life) is the strength of the glow orb's "brightness".</param>
		/// <param name="source_layer">This is the Surface that the glow orb will copy pixels from.</param>
		GlowOrb(CollidableInfo collidable_info, float player_strength, Surface* source_layer);
		
		/// <summary>
		/// Advances glow orb size change based on Phase.
		/// </summary>
		/// <param name="deltaTime">Time elapsed since last call.</param>
		void Update(float deltaTime);
		

		/// <summary>
		/// Draws glow orb within the bounds of the Surface and within the bounds of the camera view.
		/// </summary>
		/// <param name="visible_layer">The screen Surface that is visible to the player.</param>
		/// <param name="camera_left">The left edge of the Camera collision box.</param>
		/// <param name="camera_top">The top edge of the Camera collision box.</param>
		/// <param name="inbound_left">The furthest edge left of center of the Camera collision box that is inbound of the screen Surface.</param>
		/// <param name="inbound_top">The furthest edge above center of the Camera collision box that is inbound of the screen Surface.</param>
		/// <param name="inbound_right">The furthest edge right of center of the Camera collision box that is inbound of the screen Surface.</param>
		/// <param name="inbound_bottom">The furthest edge below center of the Camera collision box that is inbound of the screen Surface.</param>
		/// <param name="opacity">The Camera's current opacity value.</param>
		void Draw(Surface* visible_layer, int camera_left, int camera_top, int inbound_left, int inbound_top, int inbound_right, int inbound_bottom, float opacity = 1.0f) override;


		/// <summary>
		/// Manually changes the Phase of the glow orb. Normally used to make safe glow orbs go into waning at end of levels.
		/// </summary>
		/// <param name="new_phase">The phase to change to.</param>
		virtual void SetPhase(Phase new_phase);
		
		int GetParentId() { return m_parent_id; }
		void SetParentId(int parent_id) { m_parent_id = parent_id; }

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
		
		int m_parent_id{ 0 };

		Surface* m_source_layer;
	};
};