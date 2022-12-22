#include "temp_glow_orb.h"
#include "template.h"

#include <cstdio> //printf
#include <vector>


namespace Tmpl8
{
	TempGlowOrb::TempGlowOrb(vec2 position) :
		GlowOrb{ position, CollidableType::TEMP_GLOW }
	{
		radius_max = 50.0f;
		delay_max = 35.0f;
		delay = delay_max;
	}


	void TempGlowOrb::UpdateWaxingPhase()
	{
		if (radius >= radius_max)
		{
			phase = Phase::FULL;
		}
		else
		{
			radius += radius_delta;
			opacity = (radius / radius_max / 2) * 255;
		}
	}


	void TempGlowOrb::UpdateFullPhase()
	{
		if (--delay <= 0)
		{
			phase = Phase::WANING;
		}
	}


	void TempGlowOrb::UpdateWaningPhase()
	{
		opacity -= opacity_delta;
		opacity_delta += opacity_delta_delta;
	}


	void TempGlowOrb::UpdateEveryPhase()
	{
		opacity = Clamp(opacity, 0.0f, 255.0f);
		radius = Clamp(radius, 0.0f, radius_max);
	}
};