#pragma once

#include <vector>
#include <SDL_keycode.h>


namespace Tmpl8
{
	struct KeyboardPacket {

		KeyboardPacket(int id) :
			m_id{ id }
		{	}

		void Reset()
		{
			m_was_pressed = false;
			m_was_released = false;
		}

		int m_id{ -1 };

		bool m_was_pressed{ false };
		bool m_was_released{ false };

		bool m_is_pressed{ false };		
		bool m_is_just_pressed{ false };
		bool m_is_just_released{ false };
	};

	class KeyboardManager
	{
	public:
		KeyboardManager();

		int GetSDLMapping(int SDL_code);
		void Update();

		void Pressed(int SDL_code);
		void Released(int SDL_code);

		bool IsPressed(int SDL_code);
		bool IsJustPressed(int SDL_code);
		bool IsJustReleased(int SDL_code);

	private:
		std::vector<KeyboardPacket> keyboard_packets;
		


	};
};

