#include "glow_manager.h"

namespace Tmpl8
{
	GlowManager::GlowManager() :
		m_glow_socket{}
	{	}


	std::vector<GlowOrb>& GlowManager::GetViewportCollidables()
	{
		return m_orbs;
	}


	GlowSocket& GlowManager::GetGlowSocket()
	{
		return m_glow_socket;
	}


	void GlowManager::Update()
	{
		// Check if any orbs need to be deleted.
		// Then check if any orb should be created.
				
		RemoveExpiredGlowOrbs();
		CheckSocketForNewGlowOrbMessage();
		UpdateAllGlowOrbs();
	}


	// Private methods.


	void GlowManager::RemoveExpiredGlowOrbs()
	{		
		/*
			Credit to juanchopanza: https://stackoverflow.com/questions/15517991/search-a-vector-of-objects-by-object-attribute
		*/
		for (int i{ 0 }; i < m_orbs.size(); i++)
		{
			auto it = std::find_if(m_orbs.begin(), m_orbs.end(),
				[](GlowOrb orb) {return orb.IsExpired();});

			if (it != m_orbs.end())
			{
				printf("Removing orb\n");
				int index = std::distance(m_orbs.begin(), it);
				m_orbs.erase(m_orbs.begin() + index);
				i--;
			}
			else
			{
				break; // reached end without finding an expired orb. stop searching.
			}
		}
	}


	void GlowManager::CheckSocketForNewGlowOrbMessage()
	{
		if (m_glow_socket.HasNewMessage())
		{
			CreateGlowOrb(m_glow_socket.ReceiveMessage());
		}
	}


	void GlowManager::CreateGlowOrb(Message& message)
	{
		printf("Creating orb\n");
		m_orbs.emplace_back(message.m_orb_position);
	}


	void GlowManager::UpdateAllGlowOrbs()
	{
		for (GlowOrb& orb : m_orbs)
		{
			orb.Update();
		}
	}
};