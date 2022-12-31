#pragma once

#include "collidable.h"


namespace Tmpl8
{
	// Constants.
	constexpr char OBSTACLE_TILE{ 'X' };
	constexpr char NO_TILE{ 'O' };
	constexpr char UNREACHABLE_TILE{ '-' };
	constexpr char START_TILE{ '!' };
	constexpr char MESSAGE_TILE{ '#' };
	constexpr char EOF_MARK{ '.' };
	constexpr char UNKNOWN_MARK{ '?' };

	struct BlueprintCode
	{
		// Convert char representation of hexadecimal into decimal.
		void SetTileDetail(char tile_detail)
		{
			tile_detail -=  tile_detail < 65
				? 48 // numeral set. char value of 0 is 48.
				: (65 - 10); // alpha set. char value of A is 65. Reduce by 10 to pickup after numerals left off.
			
			m_tile_detail = tile_detail;
		}

		char m_tile_id{ '?' };
		int m_tile_detail{ 0 };
	};

};

