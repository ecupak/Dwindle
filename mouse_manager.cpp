#include "mouse_manager.h"


namespace Tmpl8
{
	void MouseManager::Update(MousePacket& mouse_packet)
	{
		if (mouse_packet.m_was_pressed)
		{			
			m_is_just_pressed = !m_is_pressed;
			m_is_pressed = true;
			m_is_just_released = false;
		}
		else if (mouse_packet.m_was_released)
		{		
			m_is_just_released = !m_is_pressed;
			m_is_pressed = false;
			m_is_just_pressed = false;
		}
		else
		{
			m_is_just_pressed = false;
			m_is_just_released = false;
		}

		m_mouse_x = mouse_packet.m_x;
		m_mouse_y = mouse_packet.m_y;

		mouse_packet.Reset();
	}
}
