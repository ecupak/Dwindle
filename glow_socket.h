#pragma once

#include "socket.h"
#include "template.h"
#include "collidable.h"


namespace Tmpl8
{
	struct SafeGlowInfo
	{
		SafeGlowInfo() :
			m_is_safe_glow_needed{ false },
			m_is_on_delay{ false }
		{	}			

		SafeGlowInfo(bool is_safe_glow_needed, bool is_on_delay, Collidable* moving_obstacle) :
			m_is_safe_glow_needed{ is_safe_glow_needed },
			m_is_on_delay{ is_on_delay },
			m_moving_obstacle{ moving_obstacle }
		{	}

		bool m_is_safe_glow_needed{ false };
		bool m_is_on_delay{ false };
		Collidable* m_moving_obstacle{ nullptr };
	};


	enum class GlowAction
	{	
		LEVEL_RESET,
		MAKE_ORB,
		REMOVE_ORB,
	};


	struct GlowMessage
	{
	public:
		// level reset
		GlowMessage(GlowAction action) :
			m_action{ action }
		{	}


		// make orb (full/safe).
		GlowMessage(GlowAction action, vec2& orb_position, float player_strength, CollidableType glow_orb_type, SafeGlowInfo safe_glow_info = SafeGlowInfo{}) :
			m_action{ action },
			m_orb_position{ orb_position },
			m_player_strength{ player_strength },
			m_glow_orb_type{ glow_orb_type },
			m_safe_glow_info{ safe_glow_info }
		{	}


		// make orb (pickup).
		GlowMessage(GlowAction action, vec2& orb_position, float player_strength, CollidableType glow_orb_type, int parent_id) :
			m_action{ action },
			m_orb_position{ orb_position },
			m_player_strength{ player_strength },
			m_glow_orb_type{ glow_orb_type },
			m_parent_id{ parent_id }
		{	}


		// remove orb.
		GlowMessage(GlowAction action, int parent_id) :
			m_action{ action },
			m_parent_id{ parent_id }
		{	}


		GlowAction m_action;
		vec2 m_orb_position{ 0.0f, 0.0f };
		float m_player_strength{ 0.0f };
		CollidableType m_glow_orb_type{ CollidableType::UNKNOWN };		
		SafeGlowInfo m_safe_glow_info;
		int m_parent_id{ 0 };
	};
};