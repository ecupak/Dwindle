#pragma once

#include <vector>

#include "collidable.h"
#include "level.h"
#include "player.h"
#include "viewport.h"
#include "collision_socket.h"

namespace Tmpl8
{
	enum class CollidableGroup
	{
		VIEWPORT,
		PLAYER,
		CAMERA,
		NONE,
	};


	class CollisionManager
	{
	public:
		// Structor.
		CollisionManager(Viewport& viewport, Player& player, Camera& camera);
		// Updates level reference for when level changes.
		void SetNewLevel(Level& current_level);		
		// Runs collision checks on viewport and player collidables and tells objects to resolve any detected collisions.
		void UpdateCollisions();
		
		Socket<CollisionMessage>& GetLevelCollisionSocket();
		Socket<CollisionMessage>& GetGlowCollisionSocket();

	private:
		// METHODS.
		
		// Builds viewport and player collidables. Called during constructor or when m_level is set.
		void CreateCollidables();
		// Each adds relevant objects to their collidables vector.
		void CreateViewportCollidables();
		void CreatePlayerCollidables();
		void CreateCameraCollidables();
		// Gets collidable info from level (glows or obstacles) and adds viewport or player object to collidables vector.
		void GetCollidablesFromLevel(CollidableGroup c_group);
		void AddUniqueElementToCollidables(CollidableGroup c_group);

		void GetCollidablesFromGlowConnection();


		// Tell viewport and player to resolve collisions.
		void CollisionManager::ProcessCollisions();

		/*
			Collision checking for all collidables in each group.
			When a colliding pair is found, each collidable is passed
			a reference to the thing it collided with.

			With a very many thanks to YulyaLesheva for the example code and explanatory graphic
			of the sweep and prune method: https://github.com/YulyaLesheva/Sweep-And-Prune-algorithm
		*/
		// Starts sequence for each collidable group.
		void RunCollisionChecks();
		// Sequence.
		void CheckForCollisions(CollidableGroup c_group);
		void SortCollidablesOnXAxis(std::vector<Collidable*>& collidables);
		std::vector<std::vector<Collidable*>> GetXAxisOverlaps(std::vector<Collidable*>& collidables);
		std::vector<std::vector<Collidable*>> GetCollisions(std::vector<std::vector<Collidable*>>& x_overlaps);
		void NotifyCollisionPairs(std::vector<std::vector<Collidable*>>& collisions);
		// Helper to determine which collidable group to check.
		std::vector<Collidable*>& GetCollidableByGroup(CollidableGroup c_group);

		// ATTRIBUTES.		
		Viewport &m_viewport;
		Player &m_player;
		Camera& m_camera;
		Level* m_level;	// Not set during constructor; has no default constructor.

		Socket<CollisionMessage> m_level_connection{};
		Socket<CollisionMessage> m_glow_connection{};

		/*
			Contains glow objects (masks for the level).
			When they collide with the viewport, we know to draw them to the screen.
		*/
		std::vector<Collidable*> m_viewport_collidables;

		/*
			Contains physical objects (things the player interacts with).
			When they collide with the player, we know to update them and/or the player.
		*/
		std::vector<Collidable*> m_player_collidables;


		std::vector<Collidable*> m_camera_collidables;

	};
};

