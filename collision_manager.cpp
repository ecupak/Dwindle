#include "collision_manager.h"

#include <cstdio> //printf
#include <algorithm>


namespace Tmpl8
{
	CollisionManager::CollisionManager(Player& player, Camera& camera) :
		m_player{ player },
		m_camera{ camera },
		m_level{ nullptr }	// Set via method after level has been created and initialized.
	{	};


	// Public methods.


	void CollisionManager::SetNewLevel(Level& current_level)
	{
		/* Update reference to the level and recreate collidables. */

		m_level = &current_level;
		CreateCollidables(CollidableGroup::CAMERA);
		CreateCollidables(CollidableGroup::PLAYER);
	}


	Socket<CollisionMessage>& CollisionManager::GetLevelCollisionSocket()
	{
		return m_level_connection;
	}

	
	Socket<CollisionMessage>& CollisionManager::GetGlowCollisionSocket()
	{
		return m_glow_connection;
	}


	void CollisionManager::UpdateCollisions(CollidableGroup c_group)
	{
		UpdateCollidables(c_group);
		CheckForCollisions(c_group);
		ProcessCollisions(c_group);
	}


	// Private methods. 


	void CollisionManager::ProcessCollisions(CollidableGroup c_group)
	{
		switch (c_group)
		{
		case CollidableGroup::CAMERA:
			m_camera.ProcessCollisions();
			break;
		case CollidableGroup::PLAYER:
			m_player.ProcessCollisions();
			break;
		}
	}


	void CollisionManager::CreateCollidables(CollidableGroup c_group)
	{
		switch (c_group)
		{
		case CollidableGroup::CAMERA:
			UpdateCameraCollidables();
			break;
		case CollidableGroup::PLAYER:
			UpdatePlayerCollidables();
			break;
		}
	}


	void CollisionManager::UpdateCollidables(CollidableGroup c_group)
	{
		switch (c_group)
		{
		case CollidableGroup::CAMERA:
			UpdateCameraCollidables();
			break;
		default:
			UpdatePlayerCollidables();
			break;
		}		
	}
	

	void CollisionManager::UpdateCameraCollidables()
	{
		/*
			Called after new level has been registered or during collision loop.
			If there is a new glow orb in the list, update list.
			Add unique item (viewport) that cares about the glow orbs.
		*/
		
		UpdateGlowOrbList();

		if (m_is_glow_orb_list_updated)
		{
			GetCollidablesFromLevel(CollidableGroup::CAMERA);
			AddUniqueElementToCollidables(CollidableGroup::CAMERA);
		}
	}


	void CollisionManager::UpdateGlowOrbList()
	{
		m_is_glow_orb_list_updated = false;

		if (m_glow_connection.HasNewMessage())
		{
			std::vector<CollisionMessage>& messages = m_glow_connection.ReadMessages();

			// If we somehow missed getting an update, get the latest update.
			m_glow_orb_collidables = messages.back().m_collidables;
			m_glow_connection.ClearMessages();
			m_is_glow_orb_list_updated = true;
		}
	}


	void CollisionManager::UpdatePlayerCollidables()
	{
		/*
			Called after new level has been registered.
			Adds obstacles to list.
			Add unique item (player) that cares about obstacles.
		*/

		GetCollidablesFromLevel(CollidableGroup::PLAYER);
		AddUniqueElementToCollidables(CollidableGroup::PLAYER);
	}


	void CollisionManager::GetCollidablesFromLevel(CollidableGroup c_group)
	{
		/* Get collidables from level. Will stay synced. */

		switch (c_group)
		{
		case CollidableGroup::CAMERA:
			m_camera_collidables = m_glow_orb_collidables;
			break;
		case CollidableGroup::PLAYER:			
			m_player_collidables = m_level->GetPlayerCollidables();			
			break;
		}
	}
	

	void CollisionManager::AddUniqueElementToCollidables(CollidableGroup c_group)
	{
		/* Add the viewport/player to the collidables list that it will interact with. */

		switch (c_group)
		{
		case CollidableGroup::CAMERA:
			m_camera_collidables.push_back(&m_camera);
			// Player collidable is a set of collidables along perimeter.
			for (DetectorPoint& point : m_player.GetCollisionPoints())
			{
				m_camera_collidables.push_back(&point);
			}
			break;
		case CollidableGroup::PLAYER:
			// Player collidable is a set of collidables along perimeter.
			for (DetectorPoint& point : m_player.GetCollisionPoints())
			{
				m_player_collidables.push_back(&point);
			}
			break;
		}
	}


	void CollisionManager::CheckForCollisions(CollidableGroup c_group)
	{
		std::vector<Collidable*>& collidables{ GetCollidableByGroup(c_group) };
		
		if (collidables.size() < 2) return;

		/*
			Pre-sort by x-axis value (left to right).
			Allows quicker comparison for the x-axis overlap check.
		*/
		SortCollidablesOnXAxis(collidables);

		/*
			Get a list of collidable pairs that overlap on the X axis,
			then use that list to get a list of collidable pairs that fully
			overlap. Notify each pair of what it has collided with.
		*/
		std::vector<std::vector<Collidable*>> x_overlaps = GetXAxisOverlaps(collidables);
		std::vector<std::vector<Collidable*>> collisions = GetCollisions(x_overlaps);
		NotifyCollisionPairs(collisions);
	}
	

	void CollisionManager::SortCollidablesOnXAxis(std::vector<Collidable*>& collidables)
	{
		/* Sort by left-most x-position of each collidable. */

		std::sort(collidables.begin(), collidables.end(),
			[=](Collidable* a, Collidable* b) { return a->left < b->left; });
	}


	std::vector<std::vector<Collidable*>> CollisionManager::GetXAxisOverlaps(std::vector<Collidable*>& collidables)
	{
		/*
			The outer loop goes through all collidables.
			
			The current collidable is the focus. After the focus has been compared to all prior foci,
			it is put in the priorFocusList and will be compared against the next focus.

			If a prior foci has no overlap with the current focus, it is removed from the list
			(because everything is sorted by their left-most x-component, the future foci will only
			move further away from it - no overlap will happen again). Otherwise, they are saved as a pair.
		*/

		std::vector<std::vector<Collidable*>> allPairs; // Collision pairs to be returned.
		std::vector<Collidable*> priorFocusList;

		for (std::size_t focus{ 0 }; focus < collidables.size(); focus++)
		{
			for (std::size_t priorFoci{ 0 }; priorFoci < priorFocusList.size(); priorFoci++)	// Go thorugh list of things to the left of the focus.
			{
				if (collidables[focus]->left > priorFocusList[priorFoci]->right)		// If focus and prior focus don't overlap, erase prior focus.
				{
					priorFocusList.erase(priorFocusList.begin() + priorFoci);
					/*
						We removed an element and the remaining elements have all shifted over. So now there is
						a new element at this same index. To check this index again, we must step back 1 first.
					*/
					priorFoci--; 
				}
				else if (collidables[focus]->m_object_type != priorFocusList[priorFoci]->m_object_type)	// We don't care if objects of the same type touch.
				{
					std::vector<Collidable*> collisionPair;
					collisionPair.push_back(collidables[focus]);
					collisionPair.push_back(priorFocusList[priorFoci]);
					allPairs.push_back(collisionPair);
				}
			}
			priorFocusList.push_back(collidables[focus]);	// Add current focus to list of prior foci to now be checked against the next foci.
		}
		return allPairs;
	}


	std::vector<std::vector<Collidable*>> CollisionManager::GetCollisions(std::vector<std::vector<Collidable*>>& x_overlaps)
	{
		/*
			Each x_overlap pair is a potential collision; now we compare their y-values.
			If collidable A's top is above B's bottom AND A's bottom is under B's top, they collide.
			Or, another way: If A's bottom is above B's top OR A's top is under B's bottom, they don't collide.
		*/

		std::vector<std::vector<Collidable*>> allPairs; // Collision pairs to be returned.

		for (std::vector<Collidable*>& potentialCollision : x_overlaps)
		{
			if (potentialCollision.front()->top <= potentialCollision.back()->bottom	// front() is A and back() is B
				&& potentialCollision.front()->bottom >= potentialCollision.back()->top)// for the example in description.
			{
				allPairs.push_back(potentialCollision);
			}
		}
		return allPairs;
	}


	void CollisionManager::NotifyCollisionPairs(std::vector<std::vector<Collidable*>>& collisions)
	{
		/* Notify each collidable it has collided, passing the other object to it. */

		for (std::vector<Collidable*>& collision : collisions)
		{
			collision.front()->ResolveCollision(collision.back());
			collision.back()->ResolveCollision(collision.front());
		}
	}


	// Private assitant methods.


	std::vector<Collidable*>& CollisionManager::GetCollidableByGroup(CollidableGroup c_group)
	{
		switch (c_group)
		{
		case CollidableGroup::CAMERA:
			return m_camera_collidables;
		case CollidableGroup::PLAYER:			
			return m_player_collidables;
		}
	}
};