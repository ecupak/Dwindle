#pragma once

#include <string>
#include <memory>
#include <vector>

#include "surface.h"

namespace Tmpl8
{
	struct LineInfo
	{
		LineInfo(std::string& line) :
			m_line{ line }
		{	}

		std::string m_line;
		int m_left_padding{ 0 };
	};

	class TextSpriteMaker
	{
	public:
		TextSpriteMaker(int tile_size, int margin);

		std::unique_ptr<Sprite> GetTextSprite(int horizontal_span, std::string& contents, Pixel text_color);

	private:
		void ResetValues();
		void FindMaxCharactersPerLine(int horizontal_span);
		void ApplyWordWrap();
		void FindMaxLengthLine();
		void SetLineIndentation();
		Surface* GetNewSurface();
		int GetContentLength();
		int GetSurfaceHeight();		
		void PrintLines(Surface* surface, Pixel text_color);

		int m_min_margin{ 0 };
		int m_tile_size{ 0 };
		int m_max_length{ 0 };
		int m_max_characters_per_line{ 0 };
		std::vector<LineInfo> m_lines;
	};
};

