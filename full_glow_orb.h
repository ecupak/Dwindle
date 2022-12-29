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
		FullGlowOrb(vec2 position, float player_strength, Surface* source_layer, Socket<GlowMessage>* glow_socket, bool is_safe_glow_needed);

	private:
		// METHODS.
		void UpdateFullPhase(float deltaTime) override;
		void UpdateWaxingPhase(float deltaTime) override;
		void UpdateWaningPhase(float deltaTime) override;
		void UpdateEveryPhase(float deltaTime) override;
		

		// ATTRIBUTES.		
		Socket<GlowMessage>* m_glow_socket;
	};
};