#pragma once

#include "collidable.h"


namespace Tmpl8
{
	// Constants.
	constexpr char NO_TILE{ '.' };
	constexpr char VISIBLE_OBSTACLE_TILE{ 'S' };
	constexpr char HIDDEN_OBSTACLE_TILE{ 'U' };
	constexpr char UNREACHABLE_OBSTACLE_TILE{ 'X' };
	constexpr char DANGEROUS_OBSTACLE_TILE{ 'W' };
	constexpr char VISIBLE_MOVING_OBSTACLE_TILE{ '+' };
	constexpr char HIDDEN_MOVING_OBSTACLE_TILE{ '-' };	
	constexpr char EASY_EXIT_TILE{ 'G' };
	constexpr char HARD_EXIT_TILE{ 'R' };
	constexpr char PICKUP_TILE{ '^' };	
	constexpr char START_TILE{ 'O' };
	constexpr char FINISH_TILE{ '#' };	
	constexpr char EOF_MARK{ '!' };
	constexpr char UNKNOWN_MARK{ '?' };

	struct BlueprintCode
	{
		// Convert char representation of hexadecimal into decimal.
		//void SetTileDetail(char tile_detail)
		//{
		//	tile_detail -=  tile_detail < 65
		//		? 48 // numeral set. char value of 0 is 48.
		//		: (65 - 10); // alpha set. char value of A is 65. Reduce by 10 to pickup after numerals left off.
		//	
		//	m_tile_detail = tile_detail;
		//}

		char m_tile_id{ '?' };
		int m_tile_detail{ 0 };
	};

};

