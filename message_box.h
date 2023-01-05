#pragma once

#include <memory>
#include <vector>

#include "surface.h"
#include "template.h"

#include "message_enums.h"
#include "text_sprite_maker.h"
#include "text_repo.h"


namespace Tmpl8
{
	class MessageBox
	{
	public:
		MessageBox(Entry entry, int TILE_SIZE, Pixel color);

		int GetExtraXPadding(int tile_span, int TILE_SIZE);
		void Draw(Surface* text_layer, float scaling_factor);
		
		MessageType m_message_type{ MessageType::GUIDE };

	private:
		TextSpriteMaker m_tsmaker;

		std::unique_ptr<Sprite> m_sprite;	
		int m_tile_size{ 0 };
		int m_extra_padding_x{ 0 };
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
	};
};

