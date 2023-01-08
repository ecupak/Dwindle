#include "button.h"

namespace Tmpl8
{
	constexpr int LETTER_HEIGHT{ 5 };
	constexpr int LETTER_WIDTH{ 6 };
	constexpr unsigned int NORMAL{ 0xFFFFFFFF };
	constexpr unsigned int HOVER{ 0xFFFF7E00 };

	Button::Button(std::string text, float scaling_factor, vec2& position, vec2& size, std::function<void()>& onclick_fn, MouseManager& mouse_manager) :
		m_text{ text },
		m_onclick_fn{ onclick_fn },
		m_mouse_manager{ mouse_manager },
		// Text layer info.
		m_text_width{ GetSurfaceWidth(m_text.length()) },
		m_text_height{ GetSurfaceHeight() },
		m_text_layer{ m_text_width, m_text_height },
		m_text_sprite{ &m_text_layer, 1, false },
		m_draw_color{ NORMAL },
		m_scaled_height{ static_cast<int>(scaling_factor * LETTER_HEIGHT) },
		m_scaled_width{ static_cast<int>(scaling_factor * m_text_width) },		
		// Button sizing / bounds.
		m_button_width{ static_cast<int>(size.x) },
		m_button_height{ static_cast<int>(size.y) },
		m_padding_x{ (m_button_width - m_scaled_width) / 2 },
		m_padding_y{ (m_button_height - m_scaled_height) / 2 },
		m_button_left{ static_cast<int>(position.x) },
		m_button_right{ m_button_left + m_button_width - 1 },
		m_button_top{ static_cast<int>(position.y) },
		m_button_bottom{ m_button_top + m_button_height - 1 }		
	{	}


	int Button::GetSurfaceWidth(int word_length)
	{
		// Letter length is 5 pixels + 1 separating pixel.
		int length = word_length * LETTER_WIDTH;

		// Don't need the final separating space.
		length -= 1;

		return length;
	}


	int Button::GetSurfaceHeight()
	{
		// Letter height is 6 pixels.
		return LETTER_HEIGHT;
	}


	bool Button::Update()
	{
		if (m_mouse_manager.m_mouse_x > m_button_left && m_mouse_manager.m_mouse_x < m_button_right
			&& m_mouse_manager.m_mouse_y > m_button_top && m_mouse_manager.m_mouse_y < m_button_bottom)
		{
			m_draw_color = HOVER;

			if (m_mouse_manager.m_is_just_pressed)
			{
				m_onclick_fn();
				return true;
			}			
		}
		else
		{
			m_draw_color = NORMAL;			
		}
		return false;
	}


	void Button::Draw(Surface* visible_layer)
	{
		m_text_layer.Clear(0x00000000);
		m_text_layer.Print(&(m_text)[0], 0, 0, m_draw_color);
		
		// Button edge.
		visible_layer->Box(m_button_left, m_button_top, m_button_right, m_button_bottom, m_draw_color);
		
		// Button text.
		m_text_sprite.DrawScaled(m_button_left + m_padding_x, m_button_top + m_padding_y, m_scaled_width, m_scaled_height, visible_layer);
	}
}