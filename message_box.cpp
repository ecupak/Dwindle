#include "message_box.h"

namespace Tmpl8
{
	MessageBox::MessageBox(std::unique_ptr<Sprite> sprite, vec2& position, int tile_size) :
		m_sprite{ std::move(sprite) },
		m_position{ position },
		m_tile_size{ tile_size },
		left{ static_cast<int>(position.x * tile_size) },
		top{ static_cast<int>(top_margin + (position.y * tile_size)) }
	{	}
	

	void MessageBox::Draw(Surface* text_layer, int scaling_factor)
	{
		int scaled_width{ m_sprite->GetWidth() * scaling_factor };
		int scaled_height{ m_sprite->GetHeight() * scaling_factor };

		right = left + (padding * 2) + scaled_width;
		bottom = top + (padding * 2) + scaled_height;

		// Text on black bg in a white border.
		text_layer->Bar(left, top, right, bottom, 0xFF000000);
		m_sprite->DrawScaled(left + padding, top + padding, scaled_width, scaled_height, text_layer);
		text_layer->Box(left, top, right, bottom, 0xFFFFFFFF);
	}
};
