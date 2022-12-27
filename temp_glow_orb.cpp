#include "temp_glow_orb.h"
#include "template.h"

#include <cstdio> //printf
#include <vector>


namespace Tmpl8
{
	TempGlowOrb::TempGlowOrb(vec2 position, Surface* source_layer) :
		GlowOrb{ position, CollidableType::TEMP_GLOW, source_layer }
	{
		radius_max = 50.0f;
		delay_max = 0.15f;
		delay = delay_max;
	}


	void TempGlowOrb::UpdateWaxingPhase(float deltaTime)
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


	void TempGlowOrb::UpdateFullPhase(float deltaTime)
	{
		delay -= deltaTime;

		if (delay <= 0)
		{
			phase = Phase::WANING;
		}
	}


	void TempGlowOrb::UpdateWaningPhase(float deltaTime)
	{
		opacity -= opacity_delta;
		opacity_delta += opacity_delta_delta;
	}


	void TempGlowOrb::UpdateEveryPhase(float deltaTime)
	{
		opacity = Clamp(opacity, 0.0f, 255.0f);
		radius = Clamp(radius, 0.0f, radius_max);
	}
};