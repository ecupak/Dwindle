#pragma once

#include <string>
#include <memory>

#include "surface.h"

namespace Tmpl8
{
	class TextSpriteMaker
	{
	public:
		static std::unique_ptr<Sprite> GetTextSprite(std::string& contents, Pixel text_color);

	private:
		TextSpriteMaker() { };

		static Surface* GetNewSurface(std::string& contents);
		static int GetSurfaceWidth(std::string& contents);
		static int GetSurfaceHeight();
		static int GetContentLengthInPixels(std::string& contents);
		static char* ConvertInt2CharPointer(int new_value);
	};
};

