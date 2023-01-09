#include "mouse_manager.h"


namespace Tmpl8
{
	void MouseManager::Update()
	{
		if (m_mouse_packet.m_was_pressed)
		{			
			m_mouse_packet.m_is_just_pressed = !m_mouse_packet.m_is_pressed;
			m_mouse_packet.m_is_pressed = true;
			m_mouse_packet.m_is_just_released = false;
		}
		
		if (m_mouse_packet.m_was_released)
		{		
			m_mouse_packet.m_is_just_released = !m_mouse_packet.m_is_pressed;
			m_mouse_packet.m_is_pressed = false;
			m_mouse_packet.m_is_just_pressed = false;
		}		

		m_mouse_packet.Reset();
	}


	void MouseManager::Pressed(int SDL_code)
	{
		if (SDL_code == SDL_BUTTON_LEFT)
		{
			m_mouse_packet.m_was_pressed = true;
		}
	}


	void MouseManager::Released(int SDL_code)
	{
		if (SDL_code == SDL_BUTTON_LEFT)
		{
			m_mouse_packet.m_was_released = true;
		}
	}


	void MouseManager::SetPosition(int x, int y)
	{
		m_mouse_packet.m_position = vec2{
			static_cast<float>(x),
			static_cast<float>(y)
		};
	}


	bool MouseManager::IsPressed()
	{
		return m_mouse_packet.m_is_pressed;
	}


	bool MouseManager::IsJustPressed()
	{
		return m_mouse_packet.m_is_just_pressed;
	}


	bool MouseManager::IsJustReleased()
	{
		return m_mouse_packet.m_is_just_released;
	}


	vec2& MouseManager::GetPosition()
	{
		return m_mouse_packet.m_position;
	}
}
