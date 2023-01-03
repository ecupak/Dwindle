#include "life_hud.h"
#include <vector>

namespace Tmpl8
{
	constexpr int LETTER_HEIGHT{ 5 };	
	constexpr int LETTER_WIDTH{ 6 };
	constexpr unsigned int DANGER_ZONE{ 3 };
	constexpr unsigned int NORMAL{ 0xFFFFFFFF };
	constexpr unsigned int WARNING{ 0xFFFFFF00 };
	constexpr unsigned int CRITICAL{ 0xFFFF0000 };
	constexpr unsigned int BLACK{ 0xFF000000 };
	
	
	LifeHUD::LifeHUD() :
		m_value_layer{ GetSurfaceWidth(m_value.length()), GetSurfaceHeight() },
		m_value_sprite{ &m_value_layer, 1, false },
		m_icon_layer{ "assets/life.png" },
		m_heart_sprite{ new Surface("assets/life.png"), 1, true },
		m_tutorial_message_layer{ GetSurfaceWidth(m_tutorial_message.length()), GetSurfaceHeight() },
		m_tutorial_message_sprite{ &m_tutorial_message_layer, 1, false }
	{
		InitializeDrawingValues();
	}


	int LifeHUD::GetSurfaceWidth(int word_length)
	{
		// Letter length is 5 pixels + 1 separating pixel.
		int length = word_length * LETTER_WIDTH;

		// Don't need the final separating space.
		length -= 1;

		return length;
	}


	int LifeHUD::GetSurfaceHeight()
	{
		// Letter height is 6 pixels.
		return LETTER_HEIGHT;
	}


	void LifeHUD::InitializeDrawingValues()
	{
		// Life box setup.
		m_value_scaled_height = m_value_scaling_factor * LETTER_HEIGHT;
		m_value_scaled_width = m_value_scaling_factor * GetSurfaceWidth(m_value.length());

		int value_height = Max(m_icon_layer.GetHeight(), m_value_scaled_height);
		
		float icon_padding = floor((value_height - m_icon_layer.GetHeight()) / 2.0f);
		float value_padding = floor((value_height - m_value_scaled_height) / 2.0f);

		m_box_start = vec2{ 20.0f, 20.0f };
		
		m_icon_start = vec2{
			m_box_start.x + m_horizontal_box_padding,
			m_box_start.y + m_vertical_box_padding + icon_padding
		};
		
		m_value_start = vec2{
			m_icon_start.x + m_icon_layer.GetWidth() + m_spacing,
			m_box_start.y + m_vertical_box_padding + value_padding
		};
		
		m_box_end = vec2{
			m_value_start.x + m_value_scaled_width + m_horizontal_box_padding,
			m_box_start.y + (m_vertical_box_padding * 2) + value_height
		};

		// Tutorial message setup.
		m_tutorial_message_scaled_height = m_tutorial_message_scaling_factor * LETTER_HEIGHT;
		m_tutorial_message_scaled_width = m_tutorial_message_scaling_factor * GetSurfaceWidth(m_tutorial_message.length());
		int value_box_length = m_box_end.x - m_box_start.x;
		int tutorial_message_padding = Max(0, static_cast<int>((value_box_length - m_tutorial_message_scaled_width) / 2.0f));

		m_tutorial_message_start = vec2{
			m_box_start.x + tutorial_message_padding,
			m_box_end.y + m_control_spacing
		};

		m_tutorial_message_end = vec2{
			m_tutorial_message_start.x + m_horizontal_box_padding + m_tutorial_message_scaled_width,
			m_tutorial_message_start.y + m_tutorial_message_scaled_height
		};
	}


	Socket<LifeMessage>* LifeHUD::GetLifeHUDSocket()
	{
		return &m_life_hub;
	}


	void LifeHUD::Update(float deltaTime)
	{
		// Update opacity values.
		if (m_flash_opacity > m_opacity)
		{
			m_flash_opacity = Max(m_flash_opacity - (deltaTime * 0.3f), m_opacity);
		}

		if (m_tutorial_message_opacity > 0.0f)
		{
			m_tutorial_message_opacity = Max(m_tutorial_message_opacity - (deltaTime * 0.3f), 0.0f);
		}

		// Check for any socket messages.
		if (m_life_hub.HasNewMessage())
		{			
			std::vector<LifeMessage> messages = m_life_hub.ReadMessages();
			m_life_hub.ClearMessages();

			for (LifeMessage& message : messages)
			{
				switch (message.m_action)
				{
				case LifeAction::TUTORIAL_SAVE:
					m_tutorial_message_opacity = 1.0f;
					[[__fallthrough]]
				case LifeAction::UPDATE:
					UpdateValueLayer(message);
					break;
				default:
					break;
				}
			}
		}
	}


	void LifeHUD::UpdateValueLayer(LifeMessage& message)
	{
		int new_value{ message.m_new_value };

		// Should not be passed a negative value. Ignore it.
		if (new_value < 0) return;
		
		// Get numbers as char.
		m_value_as_char = ConvertInt2CharPointer(new_value);

		// Update opacity (mimics player's full glow orb brightness).
		m_opacity = message.m_player_strength;

		// Briefly light up the life HUD to signify loss of life.
		m_flash_opacity = Min(m_opacity + 0.3f, 1.0f);

		// Update color when close to death.
		m_draw_color = (new_value > DANGER_ZONE) ? NORMAL : (new_value > 0 ? WARNING : CRITICAL);
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
		// Draw the main box with health/light left.
		m_value_layer.Clear(0xFF000000);
		m_value_layer.Print(m_value_as_char, 0, 0, m_draw_color, true, m_flash_opacity);

		visible_layer->Bar(m_box_start.x, m_box_start.y, m_box_end.x, m_box_end.y, BLACK);
		visible_layer->Box(m_box_start.x, m_box_start.y, m_box_end.x, m_box_end.y, m_draw_color, true, m_flash_opacity);

		DrawIcon(visible_layer);

		m_value_sprite.DrawScaled(m_value_start.x, m_value_start.y, m_value_scaled_width, m_value_scaled_height, visible_layer);

		// Draw the tutorial message underneath if life was just refilled.
		if (m_tutorial_message_opacity > 0.0f)
		{
			m_tutorial_message_layer.Clear(0x00000000);
			m_tutorial_message_layer.Print(&(m_tutorial_message)[0], 0, 0, m_draw_color, true, m_tutorial_message_opacity);
			m_tutorial_message_sprite.DrawScaled(m_tutorial_message_start.x, m_tutorial_message_start.y, m_tutorial_message_scaled_width, m_tutorial_message_scaled_height, visible_layer);
		}
	}


	void LifeHUD::DrawIcon(Surface* visible_layer)
	{
		Pixel* s_pix = m_icon_layer.GetBuffer();
		Pixel* d_pix = visible_layer->GetBuffer() + static_cast<int>(m_icon_start.x + (m_icon_start.y * visible_layer->GetPitch()));

		for (int y{ 0 }; y < m_icon_layer.GetHeight(); ++y)
		{
			for (int x{ 0 }; x < m_icon_layer.GetWidth(); ++x)
			{
				if (s_pix[x] == 0xFFFFFFFF)
				{
					d_pix[x] = MixAlpha(m_draw_color, m_flash_opacity, 0xFF000000, false);
				}				
			}
			s_pix += m_icon_layer.GetPitch();
			d_pix += visible_layer->GetPitch();
		}
	}
}
