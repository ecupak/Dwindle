#pragma once

#include <memory>
#include <vector>

#include "surface.h"
#include "template.h"


namespace Tmpl8
{
	class MessageBox
	{
	public:
		MessageBox(std::unique_ptr<Sprite> sprite, vec2& position, int tile_size);
		void Draw(Surface* text_layer, int scaling_factor);

	private:
		std::unique_ptr<Sprite> m_sprite;
		vec2 m_position{ 0.0f, 0.0f };
		int m_tile_size{ 0 };

		int top_margin{ 30 }, padding{ 5 };
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
	};
};

