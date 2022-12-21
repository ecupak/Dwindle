#pragma once

#include <vector>

#include "glow_socket.h"
#include "glow_orb.h"
#include "collision_socket.h"

namespace Tmpl8
{
	class GlowManager
	{
	public:
		GlowManager();

		std::vector<GlowOrb>& GetViewportCollidables();
		GlowSocket& GetPlayerGlowSocket();
		void CheckSocketForNewGlowOrbMessage(bool& has_new_data_for_collision_socket);
		void Update();
		void UpdateGlowOrbs(bool& has_new_data_for_collision_socket);

		void RegisterCollisionSocket(CollisionSocket& collision_socket);

	private:

		void CreateGlowOrb();
		void RemoveExpiredGlowOrb(std::vector<GlowOrb>::const_iterator index_it);

		GlowSocket m_glow_socket;
		CollisionSocket* m_collision_socket;

		std::vector<GlowOrb> m_orbs;
		std::vector<Collidable*> m_collidables;
	};
};