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
		GlowManager(Surface& obstacle_layer, Surface& map_layer);

		Socket<GlowMessage>& GetPlayerGlowSocket();
		void CheckSocketForNewGlowOrbMessage();
		void Update(float deltaTime);
		void UpdateGlowOrbs(float deltaTime);

		void RegisterCollisionSocket(Socket<CollisionMessage>& collision_socket);

	private:
		void ProcessMessages();
		void CreateGlowOrb(GlowMessage& message);
		void TriggerSafeOrbDestruction(GlowMessage& message);
		void RemoveExpiredGlowOrb(std::vector<std::shared_ptr<GlowOrb>>::const_iterator index_it);

		bool is_orb_list_changed{ false };

		Surface& m_obstacle_layer;
		Surface& m_map_layer;

		Socket<GlowMessage> m_glow_socket;
		Socket<CollisionMessage>* m_collision_socket;

		std::vector<std::shared_ptr<GlowOrb>> m_orbs;
		std::vector<Collidable*> m_collidables;
	};
};