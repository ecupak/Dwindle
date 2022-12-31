#include "player_echo.h"

namespace Tmpl8
{
	PlayerEcho::PlayerEcho(Sprite& player_sprite, float opacity) :
		m_sprite{ player_sprite },
		m_opacity{ opacity }
	{	}

	
	void PlayerEcho::ApplyDelta(vec2& delta_offset)
	{
		for (Echo& echo : echos)
		{
			echo.m_position += delta_offset;
		}

		ApplyDeltaEcho(delta_offset);
	}


	void PlayerEcho::Update(vec2& next_position, int next_frame_id)
	{	
		// Seeding the delayed positions.
		if (m_echo_count < echos.size())
		{
			echos[m_echo_count] = Echo(next_position, next_frame_id);
			++m_echo_count;
		}
		else
		{
			// Pass previous info to other Player Echo.
			UpdateEcho(echos[m_head]);
			
			// Update previous info to new info.
			echos[m_head] = Echo(next_position, next_frame_id);			

			// Advance head to next set of info (now the oldest info).
			m_head = (m_head + 1) % echos.size(); // Back to beginning if we go beyond array.
		}
	}


	void PlayerEcho::Draw(Surface* visible_layer, int camera_left, int camera_top)
	{
		// Draw most delayed echo first (will be drawn over by faster echoes and main player).
		DrawEcho(visible_layer, camera_left, camera_top);

		// Draw current echo.
		m_sprite.SetFrame(echos[m_head].m_frame_id);

		m_sprite.Draw(visible_layer, 
			echos[m_head].m_position.x - camera_left, 
			echos[m_head].m_position.y - camera_top, 
			true, m_opacity);
	}
}


