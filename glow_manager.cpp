#include "glow_manager.h"

#include <cstdio> //printf


namespace Tmpl8
{
	GlowManager::GlowManager() :
		m_glow_socket{}
	{	}


	std::vector<GlowOrb>& GlowManager::GetViewportCollidables()
	{
		return m_orbs;
	}


	GlowSocket& GlowManager::GetPlayerGlowSocket()
	{
		return m_glow_socket;
	}

	void GlowManager::RegisterCollisionSocket(CollisionSocket& collision_socket)
	{
		printf("in glow. collision socket: %p\n", &collision_socket);
		m_collision_socket = &collision_socket;
	}


	void GlowManager::Update()
	{
		// Check if a new orb should be created.
		// Then check all orbs. Delete if needed, otherwise update.
		bool is_orb_list_changed{ false };

		CheckSocketForNewGlowOrbMessage(is_orb_list_changed);
		UpdateGlowOrbs(is_orb_list_changed);

		if (is_orb_list_changed)
		{
			m_collision_socket->SendMessage(m_collidables);
		}
	}


	// Private methods.


	//void GlowManager::RemoveExpiredGlowOrbs()
	//{		
	//	/*
	//		Credit to juanchopanza: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
	//	*/
	//	for (int i{ 0 }; i < m_orbs.size(); i++)
	//	{
	//		auto it = std::find_if(m_orbs.begin(), m_orbs.end(),
	//			[](GlowOrb orb) {return orb.IsExpired();});

	//		if (it != m_orbs.end())
	//		{
	//			printf("Removing orb\n");
	//			int index = std::distance(m_orbs.begin(), it);
	//			m_orbs.erase(m_orbs.begin() + index);
	//			i--;
	//		}
	//		else
	//		{
	//			break; // reached end without finding an expired orb. stop searching.
	//		}
	//	}
	//}


	void GlowManager::UpdateGlowOrbs(bool& is_orb_list_changed)
	{
		/*
			Credit to juanchopanza: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
		*/
		m_collidables.clear();
		auto orb_it = m_orbs.begin();
		for (int index{ 0 }; index < m_orbs.size(); index++)
		{
			auto orb_it{ m_orbs.begin() + index };

			if (orb_it->IsExpired())
			{
				is_orb_list_changed = true;
				RemoveExpiredGlowOrb(m_orbs.begin() + index);
				index--;
			}
			else
			{
				orb_it->Update();
				m_collidables.push_back(&(*orb_it));
			}
		}
	}

	void GlowManager::RemoveExpiredGlowOrb(std::vector<GlowOrb>::const_iterator index_it)
	{
		printf("Removing orb\n");
		m_orbs.erase(index_it);
	}


	void GlowManager::CheckSocketForNewGlowOrbMessage(bool& is_orb_list_changed)
	{
		if (m_glow_socket.HasNewMessage())
		{
			is_orb_list_changed = true;
			CreateGlowOrb();
		}
	}


	void GlowManager::CreateGlowOrb()
	{
		printf("Creating orb\n");
		GlowMessage& message = m_glow_socket.ReceiveMessage();
		m_orbs.emplace_back(message.m_orb_position, message.m_is_from_ricochet);
	}
};