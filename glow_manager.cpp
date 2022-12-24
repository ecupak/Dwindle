#include "glow_manager.h"

#include <cstdio> //printf


namespace Tmpl8
{
	GlowManager::GlowManager() :
		m_glow_socket{ },
		m_collision_socket{ }
	{	}


	/*std::vector<GlowOrb>& GlowManager::GetViewportCollidables()
	{
		return m_orbs;
	}*/


	GlowSocket& GlowManager::GetPlayerGlowSocket()
	{
		return m_glow_socket;
	}


	void GlowManager::RegisterCollisionSocket(CollisionSocket& collision_socket)
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
			m_collision_socket->SendMessage(m_collidables);
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
		std::vector<GlowMessage> messages = m_glow_socket.ReceiveMessages();

		// Stored as shared pointer to avoid splicing. All derived classes need to be stored in a single vector.

		for (GlowMessage& message : messages)
		{
			std::shared_ptr<GlowOrb> u_orb{ };
			switch (message.m_glow_orb_type)
			{
			case CollidableType::FULL_GLOW:
			{
				std::shared_ptr<GlowOrb> tu_orb{ new FullGlowOrb{ message.m_orb_position, &m_glow_socket } };
				u_orb = std::move(tu_orb);
			}
				break;
			case CollidableType::TEMP_GLOW:
			{
				std::shared_ptr<GlowOrb> tu_orb{ new TempGlowOrb{ message.m_orb_position } };
				u_orb = std::move(tu_orb);
			}
				break;
			case CollidableType::SAFE_GLOW:
			{
				std::shared_ptr<GlowOrb> tu_orb{ new SafeGlowOrb{ message.m_orb_position } };
				u_orb = std::move(tu_orb);
			}
				break;
			}

			m_orbs.push_back(u_orb);
		}
	}
};