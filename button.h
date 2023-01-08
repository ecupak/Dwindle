#pragma once

#include <string>
#include <functional>

#include "template.h"
#include "surface.h"

#include "game_socket.h"
#include "mouse_manager.h"

namespace Tmpl8
{
	class Button
	{
	public:
		Button(std::string text, float scaling_factor, vec2& position, vec2& size, std::function<void()>& onclick_fn, MouseManager& mouse_manager);
		
		bool Update();
		void Draw(Surface* visible_layer);

	private:
		int GetSurfaceWidth(int word_length);
		int GetSurfaceHeight();

		std::string m_text;		
		std::function<void()> m_onclick_fn;
		MouseManager& m_mouse_manager;

		int m_button_width{ 0 };
		int m_button_height{ 0 };
		int m_text_width{ 0 };
		int m_text_height{ 0 };
		Surface m_text_layer;
		Sprite m_text_sprite;
		Pixel m_draw_color{ 0xFFFFFFFF };
		int m_scaled_width{ 0 };
		int m_scaled_height{ 0 };
		int m_text_left{ 0 }, m_text_right{ 0 }, m_text_top{ 0 }, m_text_bottom{ 0 };
		int m_button_left{ 0 }, m_button_right{ 0 }, m_button_top{ 0 }, m_button_bottom{ 0 };

		int m_padding_x{ 0 };
		int m_padding_y{ 0 };
	};
};

