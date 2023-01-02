#include "message_box.h"

namespace Tmpl8
{
	constexpr int MARGIN{ 8 };

	/*MessageBox::MessageBox(MessageType message_type, std::unique_ptr<Sprite> sprite, vec2& position, int tile_size) :
		m_message_type{ message_type },
		m_sprite{ std::move(sprite) },
		left{ static_cast<int>(MARGIN + (position.x * tile_size)) },
		top{ static_cast<int>(MARGIN + (position.y * tile_size)) }
	{	}*/

	MessageBox::MessageBox(MessageType message_type, Entry entry, int TILE_SIZE, Pixel color) :
		m_message_type{ message_type },
		m_tsmaker{ TILE_SIZE, MARGIN },
		m_sprite{ std::move(m_tsmaker.GetTextSprite(entry.m_span, entry.m_text, color)) },
		m_extra_padding_x{ GetExtraXPadding(entry.m_span, TILE_SIZE) },
		left{ static_cast<int>(MARGIN + m_extra_padding_x + (entry.m_position.x * TILE_SIZE)) },
		top{ static_cast<int>(MARGIN + (entry.m_position.y * TILE_SIZE)) }
	{	}


	int MessageBox::GetExtraXPadding(int tile_span, int TILE_SIZE)
	{
		int full_span = tile_span * TILE_SIZE;
		int remaining_span = full_span - (m_sprite->GetWidth() * 2) - (MARGIN *2);
		return static_cast<int>(floor(remaining_span / 2.0f));
	}


	void MessageBox::Draw(Surface* layer, float scaling_factor)
	{
		int scaled_width{ static_cast<int>(ceil(m_sprite->GetWidth() * scaling_factor)) };
		int scaled_height{ static_cast<int>(ceil(m_sprite->GetHeight() * scaling_factor)) };

		right = left + scaled_width;
		bottom = top + scaled_height;

		// Text in a white border.		
		m_sprite->DrawScaled(left, top, scaled_width, scaled_height, layer);
		if (m_message_type == MessageType::GUIDE)
		{
			layer->Box(left, top, right, bottom, 0xFFFFFFFF);
		}
	}

	
};
