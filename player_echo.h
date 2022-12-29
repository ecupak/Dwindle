#pragma once

#include "surface.h"
#include "template.h"
#include <array>

namespace Tmpl8
{
	struct Echo
	{
		Echo() { }

		Echo(vec2& position, int frame_id) :
			m_position{ position },
			m_frame_id{ frame_id }
		{	}

		vec2 m_position{ 0.0f, 0.0f };
		int m_frame_id{ 0 };
	};


	class PlayerEcho
	{
	public:
		PlayerEcho(Sprite& player_sprite, float opacity);

		void Update(vec2& current_position, int next_frame_id);
		void Draw(Surface* visible_layer, int camera_left, int camera_top);

	protected:
		virtual void UpdateEcho(Echo& echo) { }
		virtual void DrawEcho(Surface* visible_layer, int camera_left, int camera_top) { }

		Sprite& m_sprite;
		float m_opacity{ 0.0f };

		vec2 m_current_position{ 0.0f, 0.0f };
		vec2 m_prev_position{ 0.0f, 0.0f };
		
		int m_next_frame_id{ 0 };
		int m_current_frame_id{ 0 };
		
		int m_frame_delay{ 10 };
		int m_echo_count{ 0 };
		
		int m_head{ 0 };
		std::array<Echo, 10> echos;
	};
};

