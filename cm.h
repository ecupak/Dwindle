#pragma once

#include <vector>

#include "collidable.h"
#include "level.h"
#include "player.h"
#include "camera.h"
#include "collision_socket.h"

namespace Tmpl8
{
	class CollisionManager2
	{
	public:
		CollisionManager2() { };

		Socket<CollisionMessage>* GetCollisionSocket();

		void PopulateLists();
		void RunCollisionCycle();
		
		void CheckSocketForNewCollisionMessages();
		void UpdateList(CollisionMessage& collision_message, std::vector<Collidable*>& list_to_update);
		void AddToList(CollisionMessage& collision_message, std::vector<Collidable*>& list_to_add_to);
		void EnablePlayerCollisions(bool is_enabled);

		void UpdateCollidables();
		void RebuildStaticList();
		void RebuildCollidablesList();
		void RebuildResolvableList();

		void CheckForCollisions();
		void SortCollidablesOnXAxis();
		std::vector<std::vector<Collidable*>> GetXAxisOverlaps(std::vector<Collidable*>& collidables);
		std::vector<std::vector<Collidable*>> GetYAxisOverlaps(std::vector<std::vector<Collidable*>>& x_overlaps);
		void NotifyCollisionPairs();
				
		void ResolveCollisions();

		bool m_has_static_list_changed{ false };
		bool m_has_dynamic_list_changed{ false };
		bool m_is_player_collisions_enabled{ true }; // move to player class.

		std::vector<Collidable*> m_collidables;
		std::vector<Collidable*> m_player_point_collidables;
		std::vector<Collidable*> m_glow_orb_collidables;
		std::vector<Collidable*> m_obstacle_collidables;
		std::vector<Collidable*> m_pickup_collidables;
		std::vector<Collidable*> m_finish_line_collidables;
		std::vector<Collidable*> m_unique_collidables;
		std::vector<Collidable*> m_static_collidables;
		std::vector<Collidable*> m_resolvable_collidables;

		std::vector<std::vector<Collidable*>> x_and_y_overlaps;
		
		Socket<CollisionMessage> m_collision_hub;
	};
};

