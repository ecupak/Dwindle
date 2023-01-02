#include "text_sprite_maker.h"


namespace Tmpl8
{
	constexpr int LETTER_HEIGHT{ 5 };
	constexpr int LETTER_WIDTH{ 6 };
	constexpr int PADDING_X{ 5 };
	constexpr int PADDING_Y{ 5 };
	constexpr int LEADING{ static_cast<int>(LETTER_HEIGHT * 1.2f) };
	constexpr int LETTER_LIMIT{ 24 };


	TextSpriteMaker::TextSpriteMaker(int tile_size, int margin) :
		m_tile_size{ tile_size },
		m_min_margin{ margin }
	{	}


	std::unique_ptr<Sprite> TextSpriteMaker::GetTextSprite(int horizontal_span, std::string& contents, Pixel text_color)
	{		
		ResetValues();

		// Break the content into multiple lines.
		// All lines will be centered in the box.
		m_lines.emplace_back(contents);
		FindMaxCharactersPerLine(horizontal_span);
		ApplyWordWrap();
		FindMaxLengthLine();
		SetLineIndentation();

		// Make the surface.
		Surface* surface{ GetNewSurface() };
		surface->Clear(0xFF010101);	// Not pure black because the template makes that transparent.

		// Pring the lines on the surface.
		PrintLines(surface, text_color);

		// Put it on a sprite (give sprite ownership of surface ptr).		
		// And return as unique ptr.
		return std::make_unique<Sprite>(surface, 1, true);
	}


	void TextSpriteMaker::FindMaxCharactersPerLine(int horizontal_span)
	{
		int full_span = horizontal_span * m_tile_size;

		// Margin is gap from tile edge to sprite edge.
		// Padding is gap from sprite edge to printed text.
		full_span -= ((PADDING_X * 2) + (m_min_margin * 2));
		
		m_max_characters_per_line = static_cast<int>(floor(1.0f * full_span / (LETTER_WIDTH * 2))); // x2 cuz that's the scaling factor.
	}


	void TextSpriteMaker::ApplyWordWrap()
	{
		const char whitespace{ ' ' };

		for (std::size_t line_index{ 0 }; line_index < m_lines.size(); ++line_index)
		{
			LineInfo& line_info{ m_lines[line_index] };

			if (line_info.m_line.length() > m_max_characters_per_line)
			{
				// Find where a whitespace occurs.
				for (int letter_index{ m_max_characters_per_line + 1 }; letter_index >= 0; --letter_index)
				{
					// Split the line at this spot.
					if (line_info.m_line[letter_index] == whitespace)
					{
						std::string old_line{ line_info.m_line.substr(0, letter_index) }; // Start of line to before the whitespace.
						std::string new_line{ line_info.m_line.substr(letter_index + 1) }; // After whitespace to the end.

						line_info.m_line = old_line;
						m_lines.emplace_back(new_line);

						break;
					}
				}
			}
		}
	}


	void TextSpriteMaker::FindMaxLengthLine()
	{
		// Find the longest line first.
		for (LineInfo& line_info : m_lines)
		{
			if (line_info.m_line.length() > m_max_length)
			{
				m_max_length = line_info.m_line.length();
			}
		}
	}


	void TextSpriteMaker::SetLineIndentation()
	{
		// Find (in pixels) the amount of indent needed for each line to be centered.
		for (LineInfo& line_info : m_lines)
		{
			if (line_info.m_line.length() < m_max_length)
			{
				int padding{ static_cast<int>(m_max_length - line_info.m_line.length()) * LETTER_WIDTH };
				line_info.m_left_padding = static_cast<int>(padding * 0.5f);
			}
		}
	}


	Surface* TextSpriteMaker::GetNewSurface()
	{
		return new Surface{ GetContentLength(), GetSurfaceHeight() };
	};


	int TextSpriteMaker::GetContentLength()
	{
		// Letter length is 5 pixels + 1 separating pixel.
		int length = m_max_length * LETTER_WIDTH;

		// Discard the final vertical line of blank pixels.
		length -= 1;

		// Include left and right padding.
		length += PADDING_X * 2;

		return length;
	}


	int TextSpriteMaker::GetSurfaceHeight()
	{
		// Height of all the lines.
		int height = m_lines.size() * LETTER_HEIGHT;

		// Include top and bottom padding.
		height += PADDING_Y * 2;

		// Inlude the leading (space between lines).
		height += LEADING * (m_lines.size() - 1);

		return height;
	}


	void TextSpriteMaker::PrintLines(Surface* surface, Pixel text_color)
	{
		for (std::size_t i{ 0 }; i < m_lines.size(); ++i)
		{
			LineInfo& line_info{ m_lines[i] };

			surface->Print(
				&(line_info.m_line)[0],
				PADDING_X + line_info.m_left_padding,				
				PADDING_Y + ( i * (LETTER_HEIGHT + LEADING)),
				text_color
			);
		}		
	}


	void TextSpriteMaker::ResetValues()
	{
		m_lines.clear();
		m_max_length = 0;
		m_max_characters_per_line = 0;
	}
}