#pragma once

#include <string>
#include <vector>
#include <array>

#include "blueprint_code.h"


namespace Tmpl8
{
	struct BlueprintData
	{
		BlueprintData() {};

		BlueprintData(int width, int height, std::string blueprint) :
			m_width{ width },
			m_height{ height },
			m_blueprint{ blueprint }
		{	}

		int m_width{ 0 };
		int m_height{ 0 };
		std::string m_blueprint;
	};


	class Blueprints
	{
	public:
		Blueprints() {};
		bool LoadBlueprintData(int level_id);
		BlueprintCode GetNextBlueprintCode();
		int GetAutotileId(int center_x, int center_y);
		int GetBlueprintWidth();
		int GetBlueprintHeight();

	private:
		bool GetIsWallAdjacent(char adjacent_value);

		bool is_iterator_set{ false };
		std::string::iterator blueprint_it;
		BlueprintData m_loaded_blueprint_data;

		std::array<BlueprintData, 4> blueprint_data
		{
			BlueprintData{29, 38, // Tutorial: 1856 x 2432
			"XXSSSSSSSSSSSSSSXSSSSSSSSSXXX"
			"XXS............SXS.......SXXX"
			"XXS............SXS.......SXXX"
			"XXS......SSSSS.SXS.SSSSS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXSSSSSSSSXXXS.SXS.SXXXS.SXXX"
			"XXXXXXXXXXXXXS.SSS.SXXXS.SXXX"
			"XXXXXXXXXXXXXS.....SXXXS.SXXX"
			"XXXXXXXXXXXXXS.....SXXXS.SXXX"
			"XXXXXXXXXXXXXSSSSSSSXXXS.SXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXS.SXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXS.SXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXS.SXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXS.SXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXS.SXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXS.SXXX"
			"SSSSSSSSSSSSSSSSSSSSSSSS.SXXX"
			"S........................SXXX"
			"S......................S.SXXX"
			"S..............SSSS.SSSS.SXXX"
			"S.............SSXXS......SXXX"
			"S............SSXXXS......SXXX"
			"S..SSSSUSUSUSSXXXXSSSSSSSSXXX"
			"S..SXXXXXXXXXXXXXXXXXXXXXXXXX"
			"U..SXXXUUUUUUUUXXXXXXXXXXXXXX"
			"S..UXXXU......UXXXXXXXXXXXXXX"
			"U..SXXXU......UXXXXXXXXXXXXXX"
			"S..UUUUU..UUUUUXXXXXXXXXXXXXX"
			"U.........UXXXXXXXXXXXXXXXXXX"
			"S......O..UXXXXXXXXXXXXXXXXXX"
			"U..UUUUUUUUUUUUUUUUUUUUUUUUUU"
			"U......................G#...."
			"U..............^........#...."
			"UUUUUUUUUUUUUUUUUUUUUUUUSSSSS"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"},

			BlueprintData{28, 17, // Level 1: 1792 x 1088
			"XXXXXXXXXXXXXUUUUUUUUUUUUXXX"
			"XXXXXXXXXXXXXU..........UUUU"
			"XXXXXXXXXXXXXU..........G#.."
			"XXXXXXXUUUUUUU.U.........#.."
			"XXXXXXXU...UXU.U..UU.UUUUSSS"
			"UUUUUUXU.^.UUU.U........UXXX"
			"..#R.UUU.U.....U.......^UXXX"
			"..#...UU.U.....UUUUUUU..UXXX"
			"SSSU...U.UU..U.....UXU..UXXX"
			"XXXUU....UUU.U.....UXU..UXXX"
			"XXXXUU...UXU.UUU...UUU..UXXX"
			"XXXUUUUU.UUU.UUUUU...U..UXXX"
			"XXXU.............U..^U..UXXX"
			"XXXU.............U.UUU..UXXX"
			"XXXU.....UUU............UXXX"
			"XXXU.O.UUUXUUU..........UXXX"
			"XXXUSSSUXXXXXUUUUUUUUUUUUXXX"},

			BlueprintData{1, 1, "?"}, // Level 2

			BlueprintData{1, 1, "?"}, // Level 3
		};
	};
};