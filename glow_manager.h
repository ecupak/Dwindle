#pragma once

#include <vector>

#include "glow_socket.h"
#include "glow_orb.h"


namespace Tmpl8
{
	class GlowManager
	{
	public:
		GlowManager();

		std::vector<GlowOrb>& GetViewportCollidables();
		GlowSocket& GetGlowSocket();
		void CheckSocketForNewGlowOrbMessage();
		void Update();
		void GlowManager::UpdateAllGlowOrbs();

	private:

		void CreateGlowOrb(Message& message);
		void RemoveExpiredGlowOrbs();

		GlowSocket m_glow_socket;

		std::vector<GlowOrb> m_orbs;
	};
};