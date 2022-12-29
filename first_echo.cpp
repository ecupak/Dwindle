#include "first_echo.h"

namespace Tmpl8
{
	FirstEcho::FirstEcho(Sprite& player_sprite) :
		PlayerEcho{ player_sprite, 0.4f },
		m_player_echo{ player_sprite }
	{	}


	void FirstEcho::UpdateEcho(Echo& echo)
	{
		m_player_echo.Update(echo.m_position, echo.m_frame_id);
	}


	void FirstEcho::DrawEcho(Surface* visible_layer, int camera_left, int camera_top)
	{ 
		m_player_echo.Draw(visible_layer, camera_left, camera_top);
	}
}
