#pragma once

#include <vector>

#include "collidable.h"
#include "level.h"
#include "player.h"
#include "camera.h"
#include "collision_socket.h"

namespace Tmpl8
{
	enum class CollidableGroup
	{
		CAMERA,
		PLAYER,
		NONE,
	};


	class CollisionManager
	{
	public:
		// Structor.
		CollisionManager(Player& player, Camera& camera);
		// Updates level_manager reference for when level_manager changes.
		void SetNewLevel(Level& current_level);		
		// Runs collision checks on viewport and player collidables and tells objects to resolve any detected collisions.
		void UpdateCollisions(CollidableGroup c_group);
		
		void EnablePlayerCollisions(bool toggle_state);

		Socket<CollisionMessage>* GetCollisionSocket();

	private:
		// METHODS.
		void CheckSocketForNewCollisionMessages();
		void ProcessMessages();

		void UpdateGlowOrbList(CollisionMessage& collision_message);		

		// Builds viewport and player collidables. Called during constructor or when m_level is set.
		void CreateCollidables(CollidableGroup c_group);
		void UpdateCollidables(CollidableGroup c_group);
		// Each adds relevant objects to their collidables vector.
		void UpdateCameraCollidables();
		void UpdatePlayerCollidables();
		// Gets collidable info from level_manager (glows or obstacles) and adds viewport or player object to collidables vector.
		void GetCollidablesFromLevel(CollidableGroup c_group);
		void AddUniqueElementToCollidables(CollidableGroup c_group);


		// Tell viewport and player to resolve collisions.
		void CollisionManager::ProcessCollisions(CollidableGroup c_group);

		/*
			Collision checking for all collidables in each group.
			When a colliding pair is found, each collidable is passed
			a reference to the thing it collided with.

			With a very many thanks to YulyaLesheva for the example code and explanatory graphic
			of the sweep and prune method: https://github.com/YulyaLesheva/Sweep-And-Prune-algorithm
		*/
		// Sequence.
		void CheckForCollisions(CollidableGroup c_group);
		void SortCollidablesOnXAxis(std::vector<Collidable*>& collidables);
		std::vector<std::vector<Collidable*>> GetXAxisOverlaps(std::vector<Collidable*>& collidables);
		std::vector<std::vector<Collidable*>> GetCollisions(std::vector<std::vector<Collidable*>>& x_overlaps);
		void NotifyCollisionPairs(std::vector<std::vector<Collidable*>>& collisions);
		// Helper to determine which collidable group to check.
		std::vector<Collidable*>& GetCollidableByGroup(CollidableGroup c_group);

		// ATTRIBUTES.
		Player &m_player;
		Camera& m_camera;
		Level* m_level;	// Not set during constructor; has no default constructor.

		Socket<CollisionMessage> m_level_connection;
		Socket<CollisionMessage> m_collision_hub;

		/*
			Contains glow objects (masks for the level_manager).
			When they collide with the viewport, we know to draw them to the screen.
		*/
		std::vector<Collidable*> m_camera_collidables;


		/*
			Contains physical objects (things the player interacts with).
			When they collide with the player, we know to update them and/or the player.
		*/
		std::vector<Collidable*> m_player_collidables;


		std::vector<Collidable*> m_glow_orb_collidables;
		bool m_is_glow_orb_list_updated{ false };
		bool m_is_level_update_needed{ true };
		bool m_is_player_collisions_enabled{ true };
	};
};

