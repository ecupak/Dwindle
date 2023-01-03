#include "glow_orb.h"
#include "template.h"

#include <vector>


namespace Tmpl8
{
	GlowOrb::GlowOrb(vec2 position, float player_strength, CollidableType object_type, Surface* source_layer, std::vector<CollidableType> collidables_of_interest, int draw_order) :
		Collidable{ object_type, draw_order, position },
		m_source_layer{ source_layer },
		m_player_strength{ player_strength }
	{	}


	void GlowOrb::Update(float deltaTime)
	{
		UpdateByPhase(deltaTime);
		UpdateBounds();
		m_is_active = (opacity > 0.0f);
	}


	void GlowOrb::SetPhase(Phase new_phase)
	{
		phase = new_phase;
	}


	void GlowOrb::UpdateByPhase(float deltaTime)
	{
		/*
			Phase cycle: WAXING -> FULL -> WANING.			
		*/

		switch (phase)
		{
		case Phase::WAXING:
			UpdateWaxingPhase(deltaTime);
			break;
		case Phase::FULL:
			UpdateFullPhase(deltaTime);
			break;
		case Phase::WANING:
			UpdateWaningPhase(deltaTime);
			break;
		}

		UpdateEveryPhase(deltaTime);
	}


	void GlowOrb::UpdateBounds()
	{
		top = static_cast<int>(floor(center.y - radius));
		bottom = static_cast<int>(floor(center.y + radius));
		left = static_cast<int>(floor(center.x - radius));
		right = static_cast<int>(floor(center.x + radius));
	}

	
	void GlowOrb::Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		/*
			Get bounding box of orb(constrained by window size).
		*/

		// viewable_layer bounds are already clamped to window bounds and are ints.
		// use unaltered results for h_pix math.
		// v_pix needs to subtract v_left and v_top.
		int d_top = Max(top, in_top);
		int d_bottom = Min(bottom, in_bottom);
		int d_left = Max(left, in_left);
		int d_right = Min(right, in_right);
	
		/*
			Iterate over bounding box and store opacity value in pixel (in blue channel).
			Only store opacity value if the current value of pixel is less.
		*/

		Pixel* d_pix = viewable_layer->GetBuffer() + (d_left - c_left) + ((d_top - c_top) * viewable_layer->GetPitch());
		Pixel* s_pix = m_source_layer->GetBuffer() + d_left + (d_top * m_source_layer->GetPitch());


		// Precalculate values to be reused each loop. 
		int new_opacity = static_cast<int>(floor(opacity));
		int radius_squared = radius * radius;

		for (int y{ d_top }; y < d_bottom; y++)
		{
			for (int x{ 0 }; x <= (d_right - d_left); x++)
			{
				// Only adjust pixels that fall within a circle based on center and radius.
				int dist_x{ x + d_left - static_cast<int>(floor(center.x)) };
				int dist_y{ y - static_cast<int>(floor(center.y)) };
				int dist = (dist_x * dist_x) + (dist_y * dist_y);
				if (dist <= radius_squared)
				{
					// The opacity decreases (becomes more transparent) as we move from the center.
					float intensity{ 1.0f - (1.0f * dist / radius_squared) };

					DrawStep(x, d_pix, s_pix, new_opacity, intensity);
				}
			}
			d_pix += viewable_layer->GetPitch();
			s_pix += m_source_layer->GetPitch();
		}
	}


	void GlowOrb::DrawStep(int x_pos, Pixel*& destination_pix, Pixel*& source_pix, int new_opacity, float intensity)
	{		
		new_opacity *= intensity;

		int current_opacity = destination_pix[x_pos] >> 24;
		if (new_opacity > current_opacity)
			destination_pix[x_pos] = MixAlpha(source_pix[x_pos], new_opacity, 0xFF000000, true);
	}

};