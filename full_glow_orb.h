#pragma once

#include "glow_orb.h"
#include "glow_socket.h"


namespace Tmpl8
{
	// Class definition.
	class FullGlowOrb : public GlowOrb
	{
	public:
		// METHODS.		
		FullGlowOrb(vec2 position, GlowSocket* glow_socket);
		//virtual void Update() {};

	private:
		// METHODS.
		void UpdateFullPhase(float deltaTime) override;
		void UpdateWaxingPhase(float deltaTime) override;
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;
		

		// ATTRIBUTES.		
		GlowSocket* m_glow_socket;
	};
};