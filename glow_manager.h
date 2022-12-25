#pragma once

#include <vector>
#include <memory>

#include "collision_socket.h"
#include "glow_socket.h"
#include "glow_orb.h"
#include "full_glow_orb.h"
#include "temp_glow_orb.h"
#include "safe_glow_orb.h"


namespace Tmpl8
{
	class GlowManager
	{
	public:
		GlowManager();

		//std::vector<GlowOrb>& GetViewportCollidables();
		Socket<GlowMessage>& GetPlayerGlowSocket();
		void CheckSocketForNewGlowOrbMessage(bool& has_new_data_for_collision_socket);
		void Update(float deltaTime);
		void UpdateGlowOrbs(bool& has_new_data_for_collision_socket, float deltaTime);

		void RegisterCollisionSocket(Socket<CollisionMessage>& collision_socket);

	private:

		void CreateGlowOrb();
		void RemoveExpiredGlowOrb(std::vector<std::shared_ptr<GlowOrb>>::const_iterator index_it);

		Socket<GlowMessage> m_glow_socket;
		Socket<CollisionMessage>* m_collision_socket;

		std::vector<std::shared_ptr<GlowOrb>> m_orbs;
		std::vector<Collidable*> m_collidables;
	};
};