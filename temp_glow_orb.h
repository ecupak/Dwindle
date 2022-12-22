#pragma once

#include "glow_orb.h"


namespace Tmpl8
{
	// Class definition.
	class TempGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		TempGlowOrb(vec2 position);

	private:
		// METHODS.
		void UpdateFullPhase() override;
		void UpdateWaxingPhase() override;
		void UpdateWaningPhase() override;
		void UpdateEveryPhase() override;
	};
};