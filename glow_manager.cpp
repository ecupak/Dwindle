#include "glow_manager.h"


namespace Tmpl8
{
	GlowManager::GlowManager() :
		m_obstacle_layer{ nullptr },
		m_map_layer{ nullptr },
		m_game_socket{ nullptr },
		m_collision_socket{ nullptr }
	{	}


	Socket<GlowMessage>* GlowManager::GetGlowSocket()
	{
		return &m_glow_hub;
	}


	void GlowManager::RegisterGameSocket(Socket<GameMessage>* game_socket)
	{
		m_game_socket = game_socket;
	}


	void GlowManager::RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket)
	{
		m_collision_socket = collision_socket;
	}

	
	void GlowManager::Update(float deltaTime)
	{
		// Check if a new orb should be created.
		// Then check all orbs. Delete if needed, otherwise update.
		m_is_orb_list_changed = false;

		CheckSocketForNewGlowOrbMessage();
		UpdateGlowOrbs(deltaTime);

		if (m_is_orb_list_changed)
		{
			m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_ORB_LIST, m_collidables });
		}

		if (m_is_resetting_level && m_orbs.size() == 0)
		{
			m_game_socket->SendMessage(GameMessage{ GameAction::ORBS_REMOVED });
			m_is_resetting_level = false;
		}
	}


	// Private methods.


	void GlowManager::UpdateGlowOrbs(float deltaTime)
	{
		/*
			Credit to juanchopanza: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
		*/
		m_collidables.clear();
		
		for (int index{ 0 }; index < m_orbs.size(); index++)
		{
			auto orb_it{ m_orbs.begin() + index };

			if ((**orb_it).m_is_active)
			{
				(**orb_it).Update(deltaTime);
				m_collidables.push_back(&(**orb_it));
			}
			else
			{
				RemoveExpiredGlowOrb(m_orbs.begin() + index);
				index--;
				m_is_orb_list_changed = true;
			}
		}
	}


	void GlowManager::RemoveExpiredGlowOrb(std::vector<std::shared_ptr<GlowOrb>>::const_iterator index_it)
	{
		m_orbs.erase(index_it);
	}


	void GlowManager::CheckSocketForNewGlowOrbMessage()
	{
		if (m_glow_hub.HasNewMessage())
		{
			ProcessMessages();			
		}
	}


	void GlowManager::ProcessMessages()
	{
		std::vector<GlowMessage> messages = m_glow_hub.ReadMessages();
		m_glow_hub.ClearMessages();

		for (GlowMessage& message : messages)
		{
			switch (message.m_action)
			{
			case GlowAction::MAKE_ORB:
				CreateGlowOrb(message);
				m_is_orb_list_changed = true;
				break;
			case GlowAction::REMOVE_ORB:
				RemoveGlowOrb(message);
				break;
			case GlowAction::LEVEL_RESET:
				TriggerSafeOrbDestruction();
				m_is_resetting_level = true;
				break;
			}
		}
	}


	void GlowManager::CreateGlowOrb(GlowMessage& message)
	{
		// Stored as shared pointer to avoid splicing. All derived classes need to be stored in a single vector.			
		switch (message.m_glow_orb_type)
		{
		case CollidableType::FULL_GLOW:
			m_orbs.push_back(std::make_shared<FullGlowOrb>(message.m_orb_position, message.m_player_strength, m_map_layer, &m_glow_hub, message.m_is_safe_glow_needed));
			break;
		case CollidableType::TEMP_GLOW:
			m_orbs.push_back(std::make_shared<TempGlowOrb>(message.m_orb_position, message.m_player_strength, m_map_layer));
			break;
		case CollidableType::SAFE_GLOW:
			m_orbs.push_back(std::make_shared<SafeGlowOrb>(message.m_orb_position, message.m_player_strength, m_obstacle_layer));
			break;
		case CollidableType::PICKUP_GLOW:
			m_orbs.push_back(std::make_shared<SafeGlowOrb>(message.m_orb_position, message.m_player_strength, m_obstacle_layer));
			break;
		}
	}


	void GlowManager::RemoveGlowOrb(GlowMessage& message)
	{
		// Remove 1 specific orb.
		std::function<bool(std::shared_ptr<GlowOrb>& orb)> find_this{
			[=](std::shared_ptr<GlowOrb>& orb) {return (*orb).m_id == message.m_parent_id;}
		};

		FindAndRemove(find_this);
	}


	void GlowManager::TriggerSafeOrbDestruction()
	{
		m_is_resetting_level = true;

		// Remove all safe glow orbs.
		std::function<bool(std::shared_ptr<GlowOrb>& orb)> find_all{
			[=](std::shared_ptr<GlowOrb>& orb) {return (*orb).m_object_type == CollidableType::SAFE_GLOW;}
		};			

		FindAndRemove(find_all);
	}


	void GlowManager::FindAndRemove(std::function<bool(std::shared_ptr<GlowOrb>& orb)> search_fn)
	{
		auto orb_it{ m_orbs.begin() };
		while (orb_it != m_orbs.end())
		{
			orb_it = std::find_if(orb_it, m_orbs.end(), search_fn);

			if (orb_it != m_orbs.end())
			{
				(**orb_it).SetPhase(Phase::WANING);
				++orb_it;
			}
		}
	}
};