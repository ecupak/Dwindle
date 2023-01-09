#include "keyboard_manager2.h"




namespace Tmpl8
{
	keyboard_manager2::keyboard_manager2()
	{
		for (int i{ 0 }; i < 5; ++i) // 1 more than the keys we use. Index 0 is a dummy KeyboardPacket.
		{
			keyboard_packets.push_back(KeyboardPacket{ i });
		}
	}


	void keyboard_manager2::Update()
	{
		for (KeyboardPacket& keypack : keyboard_packets)
		{
			if (keypack.m_was_pressed)
			{
				keypack.m_is_just_pressed = !keypack.m_is_pressed;
				keypack.m_is_pressed = true;
				keypack.m_is_just_released = false;
			}
			
			if (keypack.m_was_released)
			{
				keypack.m_is_just_released = !keypack.m_is_pressed;
				keypack.m_is_pressed = false;
				keypack.m_is_just_pressed = false;
			}			

			keypack.Reset();
		}
	}


	void keyboard_manager2::Pressed(int SDL_code)
	{
		keyboard_packets[GetSDLMapping(SDL_code)].m_was_pressed = true;
	}


	void keyboard_manager2::Released(int SDL_code)
	{
		keyboard_packets[GetSDLMapping(SDL_code)].m_was_released = true;
	}


	bool keyboard_manager2::IsPressed(int SDL_code)
	{
		return keyboard_packets[GetSDLMapping(SDL_code)].m_is_pressed;
	}


	bool keyboard_manager2::IsJustPressed(int SDL_code)
	{
		return keyboard_packets[GetSDLMapping(SDL_code)].m_is_just_pressed;
	}
	
	
	bool keyboard_manager2::IsJustReleased(int SDL_code)
	{
		return keyboard_packets[GetSDLMapping(SDL_code)].m_is_just_released;
	}


	int keyboard_manager2::GetSDLMapping(int SDL_code)
	{
		switch (SDL_code)
		{
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_LEFT:
			return 1;
		case SDL_SCANCODE_D:
		case SDL_SCANCODE_RIGHT:
			return 2;
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_UP:
			return 3;
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_DOWN:
			return 4;
		default:
			return 0; // All other keypresses are dumped into a dummy KeyboardPacket.
		}
	}

};