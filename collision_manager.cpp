#include "collision_manager.h"

#include <cstdio> //printf
#include <algorithm>


namespace Tmpl8
{
	CollisionManager::CollisionManager(Viewport& viewport, Player& player) :
		m_viewport{ viewport },
		m_player{ player },
		m_level{ nullptr }	// Set via method after level has been created and initialized.
	{	};


	// Public methods.


	void CollisionManager::SetNewLevel(Level& current_level)
	{
		/* Update reference to the level and recreate collidables. */

		m_level = &current_level;
		CreateCollidables();
	}


	void CollisionManager::UpdateCollisions()
	{
		/* Find all collisions and resolve them. */

		RunCollisionChecks();
		ProcessCollisions();
	}


	// Private methods. 


	void CollisionManager::ProcessCollisions()
	{
		m_viewport.ProcessCollisions();
		m_player.ProcessCollisions();
	}


	void CollisionManager::RunCollisionChecks()
	{
		CheckForCollisions(CollidableGroup::VIEWPORT);
		CheckForCollisions(CollidableGroup::PLAYER);
	}


	void CollisionManager::CreateCollidables()
	{
		CreateViewportCollidables();
		CreatePlayerCollidables();
	}

	void CollisionManager::CreateViewportCollidables()
	{
		/*
			Called when a new level is loaded. Will create a copy of the list of
			glow objects that interact with the viewport, then add the viewport
			to that list.
		*/

		GetCollidablesFromLevel(CollidableGroup::VIEWPORT);
		AddUniqueElementToCollidables(CollidableGroup::VIEWPORT);
	}


	void CollisionManager::CreatePlayerCollidables()
	{
		/*
			Called when a new level is loaded. Will create a copy of the list of
			glow objects that interact with the viewport, then add the viewport
			to that list.
		*/

		GetCollidablesFromLevel(CollidableGroup::PLAYER);
		AddUniqueElementToCollidables(CollidableGroup::PLAYER);
	}


	void CollisionManager::GetCollidablesFromLevel(CollidableGroup c_group)
	{
		/* Get collidables from level. Will stay synced. */

		switch (c_group)
		{
		case CollidableGroup::VIEWPORT:
			for (GlowOrb& orb : m_level->GetViewportCollidables())
			{
				m_player_collidables.push_back(&orb);
			}
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
		case CollidableGroup::VIEWPORT:
			m_viewport_collidables.push_back(&m_viewport);
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
			/*printf("mem add in pair: %p\n", potentialCollision.front());

			if (potentialCollision.front()->m_object_type == CollidableType::PLAYER
				&& potentialCollision.front()->bottom >= 552)
				printf("mem add in pair: %p\n", potentialCollision.front());
				
			if (potentialCollision.back()->m_object_type == CollidableType::PLAYER
				&& potentialCollision.back()->bottom >= 552)
				printf("mem add in pair: %p\n", potentialCollision.front());*/

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
		case CollidableGroup::VIEWPORT:
			return m_viewport_collidables;
		case CollidableGroup::PLAYER:			
			return m_player_collidables;
		}
	}
};