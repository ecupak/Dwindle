#include "player_echo.h"

namespace Tmpl8
{
	PlayerEcho::PlayerEcho(Sprite& player_sprite, float opacity) :
		m_sprite{ player_sprite },
		m_opacity{ opacity }
	{	}


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
			/*echos[m_head].m_position = next_position;
			echos[m_head].m_frame_id = next_frame_id;*/

			// Advance head to next set of info (now the oldest info).
			++m_head;
			m_head %= echos.size(); // Back to beginning if we go beyond array.
		}
	}


	void PlayerEcho::Draw(Surface* visible_layer, int camera_left, int camera_top)
	{
		m_sprite.SetFrame(m_current_frame_id);
		m_sprite.Draw(visible_layer, 
			echos[m_head].m_position.x - camera_left, 
			echos[m_head].m_position.y - camera_top, 
			true, m_opacity);


		DrawEcho(visible_layer, camera_left, camera_top);
	}
}


