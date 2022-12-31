#pragma once

#include <vector>
#include <memory>

#include "collision_socket.h"
#include "game_socket.h"
#include "glow_socket.h"

#include "glow_orb.h"
#include "full_glow_orb.h"
#include "temp_glow_orb.h"
#include "safe_glow_orb.h"
#include "player_glow_orb.h"


namespace Tmpl8
{
	class GlowManager
	{
	public:
		//GlowManager(Surface& obstacle_layer, Surface& map_layer);
		GlowManager::GlowManager();

		Socket<GlowMessage>* GetGlowSocket();
		void CheckSocketForNewGlowOrbMessage();
		void Update(float deltaTime);
		void UpdateGlowOrbs(float deltaTime);
		void TriggerSafeOrbDestruction();

		void RegisterGameSocket(Socket<GameMessage>* game_socket);
		void RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket);

		void SetMapLayer(Surface& map_layer) { m_map_layer = &map_layer; }
		void SetObstacleLayer(Surface& obstacle_layer) { m_obstacle_layer = &obstacle_layer; }
		void SetTextLayer(Surface& text_layer);

		std::shared_ptr<PlayerGlowOrb> GetPlayerGlowOrb() { return m_player_orb; }

	private:
		void ProcessMessages();
		void CreateGlowOrb(GlowMessage& message);
		void MovePlayerOrbPosition(GlowMessage& message);
		void RemoveExpiredGlowOrb(std::vector<std::shared_ptr<GlowOrb>>::const_iterator index_it);

		bool m_is_orb_list_changed{ false };
		bool m_is_resetting_level{ false };

		std::shared_ptr<PlayerGlowOrb> m_player_orb{ nullptr };

		Surface* m_obstacle_layer{ nullptr };
		Surface* m_map_layer{ nullptr };
		Surface* m_text_layer{ nullptr };
		
		Socket<GlowMessage> m_glow_hub;
		Socket<GameMessage>* m_game_socket{ nullptr };
		Socket<CollisionMessage>* m_collision_socket{ nullptr };

		std::vector<std::shared_ptr<GlowOrb>> m_orbs;
		std::vector<Collidable*> m_collidables;
	};
};