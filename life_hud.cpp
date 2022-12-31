#include "life_hud.h"
#include <vector>

namespace Tmpl8
{
	constexpr int LETTER_HEIGHT{ 5 };	
	constexpr int LETTER_WIDTH{ 6 };
	constexpr unsigned int DANGER_ZONE{ 3 };
	constexpr unsigned int NORMAL{ 0xFFFFFFFF };
	constexpr unsigned int CRITICAL{ 0xFFFF0000 };
	constexpr unsigned int BLACK{ 0xFF000000 };
	
	
	LifeHUD::LifeHUD() :
		m_value_layer{ GetSurfaceWidth(), GetSurfaceHeight() },
		m_value_sprite{ &m_value_layer, 1, false },
		m_heart_sprite{ new Surface("assets/life.png"), 1, true }
	{
		InitializeDrawingValues();
	}


	int LifeHUD::GetSurfaceWidth()
	{
		return GetWordLength("00");
	}


	int LifeHUD::GetSurfaceHeight()
	{
		// Letter height is 6 pixels.
		return LETTER_HEIGHT;
	}


	int LifeHUD::GetWordLength(std::string word)
	{
		int length = word.length();

		// Letter length is 5 pixels + 1 separating pixel.
		length *= LETTER_WIDTH;

		// Don't need the final separating space.
		length -= 1;

		return length;
	}


	void LifeHUD::InitializeDrawingValues()
	{
		//m_calculated_scaled_height = m_scaling_factor * (LETTER_HEIGHT - 1);
		m_drawn_scaled_height = m_scaling_factor * LETTER_HEIGHT;
		m_scaled_width = m_scaling_factor * GetWordLength(m_value);

		m_height = Max(m_heart_sprite.GetHeight(), m_drawn_scaled_height);
		
		m_heart_padding = floor((m_height - m_heart_sprite.GetHeight()) / 2.0f);
		m_value_padding = floor((m_height - m_drawn_scaled_height) / 2.0f);

		m_box_start = vec2{ 20.0f, 20.0f };
		
		m_heart_start = vec2{
			m_box_start.x + m_horizontal_box_padding,
			m_box_start.y + m_vertical_box_padding + m_heart_padding
		};
		
		m_value_start = vec2{
			m_heart_start.x + m_heart_sprite.GetWidth() + m_spacing,
			m_box_start.y + m_vertical_box_padding + m_value_padding
		};
		
		m_box_end = vec2{
			m_value_start.x + m_scaled_width + m_horizontal_box_padding,
			m_box_start.y + (m_vertical_box_padding * 2) + m_height
		};
	}


	Socket<LifeMessage>* LifeHUD::GetLifeHUDSocket()
	{
		return &m_life_hub;
	}


	void LifeHUD::Update(float deltaTime)
	{
		if (m_life_hub.HasNewMessage())
		{			
			std::vector<LifeMessage> messages = m_life_hub.ReadMessages();
			m_life_hub.ClearMessages();

			// We only need the most recent message.
			// But not really any way we can get more than 1 at a time.
			UpdateValueLayer(
				messages.back().m_new_value,
				messages.back().m_player_strength
			);

			m_draw_color = (messages.back().m_new_value > DANGER_ZONE) ? NORMAL : CRITICAL;
		}

		if (m_flash_opacity > m_opacity)
		{
			m_flash_opacity = Max(m_flash_opacity - (deltaTime * 0.3f), m_opacity);
		}
	}


	void LifeHUD::UpdateValueLayer(int new_value, float new_opacity)
	{
		// Should not be passed a negative value. Ignore it.
		if (new_value < 0) return;
		
		m_value_as_char = ConvertInt2CharPointer(new_value);
		m_opacity = new_opacity;
		m_flash_opacity = Min(m_opacity + 0.3f, 1.0f);
	}

	
	char* LifeHUD::ConvertInt2CharPointer(int new_value)
	{
		m_value = "";
		if (new_value < 10)
		{
			m_value = "0";
		}
		m_value += std::to_string(new_value);

		// Credit: bobobobo from https://stackoverflow.com/questions/7352099/stdstring-to-char			
		return &m_value[0];
	}


	void LifeHUD::Draw(Surface* visible_layer)
	{
		m_value_layer.Clear(0xFF000000);
		m_value_layer.Print(m_value_as_char, 0, 0, m_draw_color, true, m_flash_opacity);

		visible_layer->Bar(m_box_start.x, m_box_start.y, m_box_end.x, m_box_end.y, BLACK);
		visible_layer->Box(m_box_start.x, m_box_start.y, m_box_end.x, m_box_end.y, m_draw_color, true, m_flash_opacity);

		m_heart_sprite.Draw(visible_layer, m_heart_start.x, m_heart_start.y, true, m_flash_opacity);

		m_value_sprite.DrawScaled(m_value_start.x, m_value_start.y, m_scaled_width, m_drawn_scaled_height, visible_layer);
	}
}
