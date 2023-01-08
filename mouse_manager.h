#pragma once

namespace Tmpl8
{
	struct MousePacket
	{
		void Reset()
		{			
			bool m_was_pressed{ false };
			bool m_was_released{ false };
		}
		
		int m_x{ 0 };
		int m_y{ 0 };
		bool m_was_pressed{ false };
		bool m_was_released{ false };
	};


	class MouseManager
	{
	public:
		MouseManager() { };

		void Update(MousePacket& mouse_packet);

		bool m_is_pressed{ false };
		bool m_is_just_pressed{ false };
		bool m_is_just_released{ false };
		int m_mouse_x{ 0 };
		int m_mouse_y{ 0 };

	private:
	};
};

