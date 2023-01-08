#include "collision_manager.h"

#include <cstdio> //printf
#include <algorithm>


namespace Tmpl8
{
	Socket<CollisionMessage>* CollisionManager::GetCollisionSocket()
	{
		return &m_collision_hub;
	}


	void CollisionManager::PopulateLists()
	{
		CheckSocketForNewCollisionMessages();

		UpdateCollidables();
	}


	void CollisionManager::RunCollisionCycle()
	{
		CheckSocketForNewCollisionMessages();

		UpdateCollidables();
		CheckForCollisions();
		ResolveCollisions();
	}


	void CollisionManager::CheckSocketForNewCollisionMessages()
	{
		std::vector<CollisionMessage> messages = m_collision_hub.ReadMessages();
		m_collision_hub.ClearMessages();

		m_has_static_list_changed = false;
		m_has_dynamic_list_changed = false;
		for (CollisionMessage& message : messages)
		{
			switch (message.m_action)
			{
			
			case CollisionAction::UPDATE_PLAYER_POINT_LIST: // From glow manager.
				UpdateList(message, m_player_point_collidables);
				m_has_static_list_changed = true;
				break;
			case CollisionAction::UPDATE_OBSTACLE_LIST: // From level manager.
				UpdateList(message, m_obstacle_collidables);
				m_has_static_list_changed = true;
				break;
			case CollisionAction::UPDATE_PICKUP_LIST: // From level manager.
				UpdateList(message, m_pickup_collidables);
				m_has_static_list_changed = true;
				break;
			case CollisionAction::UPDATE_FINISH_LINE_LIST: // From level manager.
				UpdateList(message, m_finish_line_collidables);
				m_has_static_list_changed = true;
				break;
			case CollisionAction::UPDATE_UNIQUE_LIST: // From multiple.
				AddToList(message, m_unique_collidables);
				m_has_static_list_changed = true;
				break;
			case CollisionAction::UPDATE_ORB_LIST: // From glow manager.
				UpdateList(message, m_glow_orb_collidables);
				m_has_dynamic_list_changed = true;
				break;
			}
		}
	}


	void CollisionManager::UpdateList(CollisionMessage& collision_message, std::vector<Collidable*>& list_to_update)
	{
		list_to_update = collision_message.m_collidables;
	}


	void CollisionManager::AddToList(CollisionMessage& collision_message, std::vector<Collidable*>& list_to_add_to)
	{
		list_to_add_to.push_back(collision_message.m_collidable);
	}


	void CollisionManager::UpdateCollidables()
	{
		if (m_has_static_list_changed)
		{
			RebuildStaticList();
			RebuildResolvableList();
		}

		if (m_has_dynamic_list_changed || m_has_static_list_changed)
		{
			RebuildCollidablesList();
		}

	}


	void CollisionManager::RebuildStaticList()
	{
		// Credit to Kirill V. Lyadvinsky at https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors

		// Preallocate memory.
		m_static_collidables.clear();
		m_static_collidables.reserve(
			m_player_point_collidables.size()
			+ m_obstacle_collidables.size()
			+ m_pickup_collidables.size()
			+ m_finish_line_collidables.size()
			+ m_unique_collidables.size()
		);

		// Insert all into single vector.		
		m_static_collidables.insert(m_static_collidables.end(), m_player_point_collidables.begin(), m_player_point_collidables.end());
		m_static_collidables.insert(m_static_collidables.end(), m_obstacle_collidables.begin(), m_obstacle_collidables.end());
		m_static_collidables.insert(m_static_collidables.end(), m_pickup_collidables.begin(), m_pickup_collidables.end());
		m_static_collidables.insert(m_static_collidables.end(), m_finish_line_collidables.begin(), m_finish_line_collidables.end());
		m_static_collidables.insert(m_static_collidables.end(), m_unique_collidables.begin(), m_unique_collidables.end());
	}


	void CollisionManager::RebuildResolvableList()
	{
		// Credit to Kirill V. Lyadvinsky at https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors

		// Preallocate memory.
		m_resolvable_collidables.clear();
		m_resolvable_collidables.reserve(			
			m_pickup_collidables.size()			
			+ m_unique_collidables.size()
		);

		// Insert all into single vector.
		m_resolvable_collidables.insert(m_resolvable_collidables.end(), m_pickup_collidables.begin(), m_pickup_collidables.end());
		m_resolvable_collidables.insert(m_resolvable_collidables.end(), m_unique_collidables.begin(), m_unique_collidables.end());
	}


	void CollisionManager::RebuildCollidablesList()
	{
		// Credit to Kirill V. Lyadvinsky at https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors

		// Preallocate memory.
		m_collidables.clear();
		m_collidables.reserve(
			m_static_collidables.size()
			+ m_glow_orb_collidables.size()
		);

		// Insert all into single vector.
		m_collidables.insert(m_collidables.end(), m_static_collidables.begin(), m_static_collidables.end());
		m_collidables.insert(m_collidables.end(), m_glow_orb_collidables.begin(), m_glow_orb_collidables.end());
	}


	void CollisionManager::CheckForCollisions()
	{
		/*
			Pre-sort by x-axis value (left to right).
			Allows quicker comparison for the x-axis overlap check.
		*/
		SortCollidablesOnXAxis();

		/*
			Get a list of collidable pairs that overlap on the X axis,
			then use that list to get a list of collidable pairs that fully
			overlap. Notify each pair of what it has collided with.
		*/
		std::vector<std::vector<Collidable*>> x_overlaps = GetXAxisOverlaps(m_collidables);
		x_and_y_overlaps = GetYAxisOverlaps(x_overlaps);
		NotifyCollisionPairs();
	}


	void CollisionManager::SortCollidablesOnXAxis()
	{
		/* Sort by left-most x-position of each collidable. */

		std::sort(m_collidables.begin(), m_collidables.end(),
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
			// If focus is active, check for collisions. Otherwise, skip it (don't add to priorFocusList).
			if (collidables[focus]->m_is_active)
			{
				for (std::size_t priorFoci{ 0 }; priorFoci < priorFocusList.size(); priorFoci++)	// Go through list of things to the left of the focus.
				{					
					// If focus and prior focus don't overlap, erase prior focus.
					if (collidables[focus]->left > priorFocusList[priorFoci]->right)
					{
						priorFocusList.erase(priorFocusList.begin() + priorFoci);
						/*
							We removed an element and the remaining elements have all shifted over. So now there is
							a new element at this same index. To check this index again, we must step back 1 first.
						*/
						priorFoci--;
					}
					// Register overlap as long as collision id of priorFoci is in list of wanted collision ids of focus.
					else if ((collidables[focus]->m_collision_layer & priorFocusList[priorFoci]->m_collision_mask)
						|| (collidables[focus]->m_collision_mask & priorFocusList[priorFoci]->m_collision_layer))
					{
						std::vector<Collidable*> collisionPair;
						collisionPair.push_back(collidables[focus]);
						collisionPair.push_back(priorFocusList[priorFoci]);
						allPairs.push_back(collisionPair);
					}
				}
				// Add current focus to list of prior foci to now be checked against the next foci.
				priorFocusList.push_back(collidables[focus]);
			}
		}
		return allPairs;
	}


	std::vector<std::vector<Collidable*>> CollisionManager::GetYAxisOverlaps(std::vector<std::vector<Collidable*>>& x_overlaps)
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


	void CollisionManager::NotifyCollisionPairs()
	{
		/* Notify each collidable it has collided, passing the other object to it. */

		for (std::vector<Collidable*>& collision : x_and_y_overlaps)
		{
			collision.front()->RegisterCollision(collision.back());
			collision.back()->RegisterCollision(collision.front());
		}

		x_and_y_overlaps.clear();
	}


	void CollisionManager::ResolveCollisions()
	{	
		for (Collidable*& collidable : m_resolvable_collidables)
		{
			collidable->ResolveCollisions();
		}
	}
};