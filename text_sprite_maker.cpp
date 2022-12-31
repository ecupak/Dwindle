#include "text_sprite_maker.h"


namespace Tmpl8
{
	constexpr int LETTER_HEIGHT{ 5 };
	constexpr int LETTER_WIDTH{ 6 };

	std::unique_ptr<Sprite> TextSpriteMaker::GetTextSprite(std::string& contents, Pixel text_color)
	{
		// Make the surface with text.
		Surface* surface{ GetNewSurface(contents) };
		surface->Clear(0x00000000);
		surface->Print(&contents[0], 0, 0, text_color);

		// Put it on a sprite (give sprite ownership of surface ptr).		
		// And return as unique ptr.
		return std::make_unique<Sprite>(surface, 1, true);
	}


	Surface* TextSpriteMaker::GetNewSurface(std::string& contents)
	{
		return new Surface{ GetSurfaceWidth(contents), GetSurfaceHeight() };
	};
	

	int TextSpriteMaker::GetSurfaceWidth(std::string& contents)
	{
		return GetContentLengthInPixels(contents);
	}


	int TextSpriteMaker::GetContentLengthInPixels(std::string& contents)
	{
		int length = contents.length();

		// Letter length is 5 pixels + 1 separating pixel.
		length *= LETTER_WIDTH;

		// Don't need the final separating space.
		length -= 1;

		return length;
	}


	int TextSpriteMaker::GetSurfaceHeight()
	{
		return LETTER_HEIGHT;
	}
}