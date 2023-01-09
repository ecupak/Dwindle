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
		int GetDirectionId(int center_x, int center_y);

		int GetBlueprintWidth();
		int GetBlueprintHeight();

	private:
		bool GetIsWallAdjacent(char adjacent_value);
		bool HasDirectionInfo(int& direction_id, int x, int y, int center_y);

		bool is_iterator_set{ false };
		std::string::iterator blueprint_it;
		BlueprintData m_loaded_blueprint_data;

		std::array<BlueprintData, 5> blueprint_data
		{
			BlueprintData{29, 47, // Tutorial: 1856 x 3008
			"XXSSSSSSSSSSSSSSXSSSSSSSSSXXX"
			"XXS............SXS.......SXXX"
			"XXS............SXS.......SXXX"
			"XXS......SSSSS.SXS.SSSSS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXS......SXXXS.SXS.SXXXS.SXXX"
			"XXS.O....SXXXS.SXS.SXXXS.SXXX"
			"XXSSSSSSSSXXXS.SSS.SXXXS.SXXX"
			"XXXXXXXXXXXXXS.....SXXXS.SXXX"
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
			"S..SSUSUSUSUSSXXXXSSSSSSSSXXX"
			"S..UXXXXXXXXXXXXXXXXXXXXXXXXX"
			"U..SXXXXXXXXXXXXXXXXXXXXXXXXX"
			"S..UXXXUUUUUUXXXXXXXXXXXXXXXX"
			"U..SXXUU....UXXXXXXXXXXXXXXXX"
			"S..UUUU.....UXXXXXXXXXXXXXXXX"
			"U........UUUUXXXXXXXXXXXXXXXX"
			"S........UXXXXXXXXXXXXXXXXXXX"
			"U..UUUUUUUUUUUUUUUUUUUUUUUUUX"
			"U..........................UX"
			"U..............^...........UX"
			"UUUUUUUUUUUUUUUUUUUUUUUU...UX"
			"XXXXXXXXXXXXXXXXXXXXXXXU...UX"
			"XXXXXXXXXXXXXXXXXXXXXXXU...UX"
			"XXXXXXXXXXXXXXXXXXXXXXUU...UX"
			"XXXXXXXXXUUUUUUUUUUUUUU....UX"
			"UUUUUUUUUU.................UX"
			".........#R................UX"
			".........#..............SSSUX"
			"SSSSSSSSSSUWWWWWWWWWWWSSSXXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXXX"},


			BlueprintData{28, 17, // Level 1: 1792 x 1088
			"XXXXXXXXXXXXXUUUUUUUUUUUUXXX"
			"XXXXXXXXXXXXXU...........UUU"
			"XXXXXXXXXXXXXU..........G#.."
			"XXXXXXXUUUUUUU.U.........#.."
			"XXXXXXXU...UXU.U..UUUUUUUSSS"
			"UUUUUUXU...UUU.U..U.....UXXX"
			"..#R.UUU.U.....U.^U....^UXXX"
			"..#...UU.U.....U..U..U..UXXX"
			"SSSU...U.UU..UUU.....U..UXXX"
			"XXXUU....UUU.UXU.....U..UXXX"
			"XXXXUU..^UXU.UUUUU.UUU..UXXX"
			"XXXUUUUU.UUU.....U.UXU..UXXX"
			"XXXU.............U.UXU..UXXX"
			"XXXU.............U.UUU..UXXX"
			"XXXU.....UU.............UXXX"
			"XXXU.O.UUUUUU...........UXXX"
			"XXXUSSSUXXXXUUUUUUUUUUUUUXXX"},


			BlueprintData{28, 30, // Level 2: 1792 x  1920
			"XXXXXXXXXUUUUUUUUUUUUUUUUXXX"
			"XXXXXXXXXU..............UXXX"
			"XXXXXXXXXU....^.........UXXX"
			"XXXXXXXXXU.UUUUUWWWWWU..UXXX"
			"XXXXXXXXXU...UU.........UXXX"
			"XXXXXXXXXU..UUU.........UXXX"
			"XXXXXXXXXU...UU.........UXXX"
			"XXXXXXXXXU...UU.........UXXX"
			"XUUUUUUUUU..UUU.........UXXX"
			"XU......UU...UU.....^...UXXX"
			"XU..O...UU.UUUU..U.UU.SSUXXX"
			"XUSSSS..UU...UU..U.UU.UXXXXX"
			"XU......UU.^.UU..U.UU.UXXXXX"
			"XU......UU...UU..UWUUWUXXXXX"
			"XU..UWWWUUUU.UU..UUUUUUUUUUU"
			"XU...........UU.........G#.."
			"XU...........UU....^.....#.."
			"XUUUUUUUWWWWWUUWWWUU.UUUUSSS"
			"XXXXXXXXXXXXXXXXXXXU.UXXXXXX"
			"XXXUUUUUUUUUUUUUUUXU.UUUUUXX"
			"XXXU.............UXU.....UXX"
			"XXUU.............UXU.....UXX"
			"UUU..............UUUUUUU.UXX"
			"..#R...................U.UXX"
			"..#....................U.UXX"
			"SSSUUU........9+.UUU.....UXX"
			"XXXU.............UXU.....UXX"
			"XXXU.............UXUUUUUUUXX"
			"XXXUWWWWWWWWWWWWWUXXXXXXXXXX"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXX"},


			BlueprintData{28, 30, // Level 3: 1792 x 1920
			"UUUUUUUUUUUUUUUUUUUUUUUUUUUU"	
			"..#R.....................UXX"
			"..#....................^.UXX"
			"SSSUU.-5.......5-.-5....UUXX"
			"XXUU....................UXXX"
			"XXU...^.................UXXX"
			"XXU..UU.+5....WWWWWWW.S.UXXX"
			"XXU.....................UUUU"
			"XXU............^...........U"
			"XXUU.-8.......UUUUU.......^U"
			"XXU...............UUU.S.U..U"
			"XXU..^.........^..UXU.U.U..U"
			"XXU.UU.+6.....UU..UUU.U.U..U"
			"XXU............U...........U"
			"XXU............U...........U"
			"XXUUUUUU..UW..WU.........UUU"
			"XXXXXXXU..UW.^WUS.-6....G#.."
			"XXXXXXXU..UW..WUW........#.."
			"XXUUUUUU..UW..WUW.....WUUSSS"
			"XXU.......UW..WUW.....WXXXXX"
			"XXU...^...UW.^WUWWWWWWWXXXXX"
			"XXU..UUUUUUW..WUUUUUUUUUUXXX"
			"XXU.....................UUXX"
			"XXU......................UXX"
			"XXU....^...........^.....UXX"
			"XXUU.-4...UWWWWW.+4...U..UXX"
			"XXU......................UXX"
			"XXU..O...................UXX"
			"XXUUSSSUUUUUUUUUUUUUUUUUUUXX"
			"XXXXXXXXXXXXXXXXXXXXXXXXXXXX"},


			BlueprintData{ 10, 10, // GameOver Screen: 640 x 640.
			".........."
			".........."
			".........."
			".........."
			".........."
			".........."
			".........."
			".........."
			".........."
			".........." }
		};
	};
};