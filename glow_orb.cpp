#include "glow_orb.h"
#include "template.h"

#include <cstdio> //printf
#include <vector>


namespace Tmpl8
{
	GlowOrb::GlowOrb(vec2 position, bool is_from_ricochet)
	{
		center = position;
		radius = 1;
		delay = delay_max;
		m_object_type = (is_from_ricochet ? CollidableType::QUICK_GLOW : CollidableType::FULL_GLOW);
	}


	void GlowOrb::Update()
	{
		UpdateSizeAndOpacity();
		UpdateBounds();
		is_expired = (opacity <= 0.0f);
	}


	void GlowOrb::UpdateSizeAndOpacity()
	{
		/*
			Phase cycle: WAXING -> FULL -> WANING.
			While waxing, opacity and radius increase together.
			When full, if not a ricochet hit, spawn a safety orb.
			While waning, only opacity decreases.
		*/

		if (phase == Phase::FULL)
		{
			if (--delay <= 0)
				phase = Phase::WANING;
		}
		else if (phase == Phase::WAXING)
		{
			if (radius >= radius_max)
				phase = Phase::FULL;
			else
			{
				radius += radius_delta;
				opacity = (radius / radius_max) * 255;
			}
		}
		else if (phase == Phase::WANING)
		{
			opacity -= opacity_delta;
			opacity_delta += opacity_delta_delta;
		}
		opacity = Clamp(opacity, 0.0f, 255.0f);
		radius = Clamp(radius, 0.0f, radius_max);
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
					// Get alpha channel value.
					int current_opacity = v_pix[x] >> 24;
					if (new_opacity > current_opacity)
						v_pix[x] = MixAlpha(h_pix[x], new_opacity, 0xFF000000);
				}
			}
			v_pix += viewable_screen->GetPitch();
			h_pix += hidden_screen->GetPitch();
		}
	}

};