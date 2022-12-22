#include "glow_orb.h"
#include "template.h"

#include <vector>


namespace Tmpl8
{
	GlowOrb::GlowOrb(vec2 position, CollidableType object_type)
	{		
		center = position;
		m_object_type = object_type;
	}


	void GlowOrb::Update()
	{
		UpdateByPhase();
		UpdateBounds();
		is_expired = (opacity <= 0.0f);
	}


	void GlowOrb::UpdateByPhase()
	{
		/*
			Phase cycle: WAXING -> FULL -> WANING.			
		*/

		switch (phase)
		{
		case Phase::WAXING:
			UpdateWaxingPhase();
			break;
		case Phase::FULL:
			UpdateFullPhase();
			break;
		case Phase::WANING:
			UpdateWaningPhase();
			break;
		}

		UpdateEveryPhase();
	}


	void GlowOrb::UpdateBounds()
	{
		top = static_cast<int>(floor(center.y - radius));
		bottom = static_cast<int>(floor(center.y + radius));
		left = static_cast<int>(floor(center.x - radius));
		right = static_cast<int>(floor(center.x + radius));
	}


	void GlowOrb::Draw(Surface* viewable_screen, Surface* hidden_screen, int v_left, int v_top, int v_right, int v_bottom)
	{
		/*
			Get bounding box of orb(constrained by window size).
		*/

		// viewable_screen bounds are already clamped to window bounds and are ints.
		// use unaltered results for h_pix math.
		// v_pix needs to subtract v_left and v_top.
		int d_top = Max(top, v_top);
		int d_bottom = Min(bottom, v_bottom);
		int d_left = Max(left, v_left);
		int d_right = Min(right, v_right);
	
		/*
			Iterate over bounding box and store opacity value in pixel (in blue channel).
			Only store opacity value if the current value of pixel is less.
		*/

		Pixel* v_pix = viewable_screen->GetBuffer() + (d_left - v_left) + ((d_top - v_top) * viewable_screen->GetPitch());
		Pixel* h_pix = hidden_screen->GetBuffer() + d_left + (d_top * hidden_screen->GetPitch());


		// Precalculate values to be reused each loop. 
		int new_opacity = static_cast<int>(floor(opacity));
		int radius_squared = radius * radius;

		for (int y{ d_top }; y <= d_bottom; y++)
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
					float intensity{ 1.0f - (1.0f * dist / radius_squared * 2.0f) };

					DrawStep(x, v_pix, h_pix, new_opacity, intensity);
				}
			}
			v_pix += viewable_screen->GetPitch();
			h_pix += hidden_screen->GetPitch();
		}
	}


	void GlowOrb::DrawStep(int x_pos, Pixel*& visible_pix, Pixel*& hidden_pix, int new_opacity, float intensity)
	{		
		new_opacity *= intensity;

		int current_opacity = visible_pix[x_pos] >> 24;
		if (new_opacity > current_opacity)
			visible_pix[x_pos] = MixAlpha(hidden_pix[x_pos], new_opacity, 0xFF000000, true);
	}

};