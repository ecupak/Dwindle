#include "glow_orb.h"
#include "template.h"

#include <cstdio> //printf
#include <vector>


namespace Tmpl8
{
	GlowOrb::GlowOrb(vec2 position) :
		center{ position }
	{
		radius = 1;
		delay = delay_max;
	}


	bool GlowOrb::Update()
	{
		UpdateSizeAndOpacity();

		if (opacity <= 0.0f)
			is_expired = true;
		else
		{
			//UpdateBounds(m_glow_orb_layer);
			//DrawGlowOrb(m_glow_orb_layer);
		}

		return is_expired;
	}


	void GlowOrb::UpdateSizeAndOpacity()
	{
		/*
			Mode cycle: WAXING -> FULL -> WANING.
			While waxing, opacity and radius increase together.
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
				opacity = (radius / radius_max) * 100;
			}
		}
		else if (phase == Phase::WANING)
		{
			opacity -= opacity_delta;
			opacity_delta += opacity_delta_delta;
		}
		opacity = Clamp(opacity, 0.0f, 100.0f);
		radius = Clamp(radius, 0.0f, radius_max);
	}

	//
	//void GlowOrb::UpdateBounds(Surface* m_glow_orb_layer)
	//{
	//	/*
	//		Get bounding box of orb(constrained by window size).
	//	*/

	//	float r = floor(radius); // Bounds should be an integer.
	//	top = Clamp(static_cast<int>(center.y - r), 0, m_glow_orb_layer->GetHeight() - 1);
	//	bottom = Clamp(static_cast<int>(center.y - r), 0, m_glow_orb_layer->GetHeight() - 1);
	//	left = Clamp(static_cast<int>(center.x - r), 0, m_glow_orb_layer->GetWidth() - 1);
	//	right = Clamp(static_cast<int>(center.x - r), 0, m_glow_orb_layer->GetWidth() - 1);
	//}


	//void GlowOrb::DrawGlowOrb(Surface* m_glow_orb_layer)
	//{
	//	/*
	//		Iterate over bounding box and store opacity value in pixel (in blue channel).
	//		Only store opacity value if the current value of pixel is less.
	//	*/

	//	Pixel* pixel = m_glow_orb_layer->GetBuffer() + left + (top * m_glow_orb_layer->GetPitch());

	//	// Precalculate values to be reused each loop.
	//	int new_opacity = static_cast<int>(opacity);
	//	int radius_squared = radius * radius;

	//	for (int y{ top }; y <= bottom; y++)
	//	{
	//		for (int x{ 0 }; x <= (right - left); x++)
	//		{
	//			// Only adjust pixels that fall within a circle based on center and radius.
	//			int dist_x{ x + left - center.x };
	//			int dist_y{ y - center.y };
	//			int dist = (dist_x * dist_x) + (dist_y * dist_y);
	//			if (dist <= radius_squared)
	//			{
	//				int current_opacity = pixel[x] & 255;
	//				if (new_opacity > current_opacity)
	//					pixel[x] = new_opacity;
	//			}
	//		}
	//		pixel += m_glow_orb_layer->GetPitch();
	//	}
	//}

};