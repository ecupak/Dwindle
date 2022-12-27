#include "glow_manager.h"

#include <cstdio> //printf


namespace Tmpl8
{
	GlowManager::GlowManager(Surface& obstacle_layer, Surface& map_layer) :
		m_glow_socket{ },
		m_collision_socket{ },
		m_obstacle_layer{ obstacle_layer },
		m_map_layer{ map_layer }
	{	}


	Socket<GlowMessage>& GlowManager::GetPlayerGlowSocket()
	{
		return m_glow_socket;
	}


	void GlowManager::RegisterCollisionSocket(Socket<CollisionMessage>& collision_socket)
	{
		m_collision_socket = &collision_socket;
	}


	void GlowManager::Update(float deltaTime)
	{
		// Check if a new orb should be created.
		// Then check all orbs. Delete if needed, otherwise update.
		bool is_orb_list_changed{ false };

		CheckSocketForNewGlowOrbMessage(is_orb_list_changed);
		UpdateGlowOrbs(is_orb_list_changed, deltaTime);

		if (is_orb_list_changed)
		{
			m_collision_socket->SendMessage(CollisionMessage{ m_collidables });
		}
	}


	// Private methods.


	void GlowManager::UpdateGlowOrbs(bool& is_orb_list_changed, float deltaTime)
	{
		/*
			Credit to juanchopanza: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
		*/
		m_collidables.clear();
		auto orb_it = m_orbs.begin();
		for (int index{ 0 }; index < m_orbs.size(); index++)
		{
			auto orb_it{ m_orbs.begin() + index };

			if ((**orb_it).IsExpired())
			{
				is_orb_list_changed = true;
				RemoveExpiredGlowOrb(m_orbs.begin() + index);
				index--;
			}
			else
			{
				(**orb_it).Update(deltaTime);
				m_collidables.push_back(&(**orb_it));
			}
		}
	}

	void GlowManager::RemoveExpiredGlowOrb(std::vector<std::shared_ptr<GlowOrb>>::const_iterator index_it)
	{
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
		std::vector<GlowMessage> messages = m_glow_socket.ReadMessages();
		m_glow_socket.ClearMessages();

		// Stored as shared pointer to avoid splicing. All derived classes need to be stored in a single vector.

		for (GlowMessage& message : messages)
		{	
			switch (message.m_glow_orb_type)
			{
			case CollidableType::FULL_GLOW:
				m_orbs.push_back(std::make_shared<FullGlowOrb>(message.m_orb_position, &m_map_layer, &m_glow_socket, message.m_is_safe_glow_needed));
				break;
			case CollidableType::TEMP_GLOW:
				m_orbs.push_back(std::make_shared<TempGlowOrb>(message.m_orb_position, &m_map_layer));
				break;
			case CollidableType::SAFE_GLOW:
				m_orbs.push_back(std::make_shared<SafeGlowOrb>(message.m_orb_position, &m_obstacle_layer));
				break;
			}
		}
	}
};