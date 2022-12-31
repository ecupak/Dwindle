#pragma once

#include <string>
#include <array>

#include "blueprint_code.h"


namespace Tmpl8
{
	class Blueprints
	{
	public:
		Blueprints() {};
		bool LoadBlueprint(int level_id);
		BlueprintCode GetNextBlueprintCode();
		int GetAutotileId(int center_x, int center_y, int blueprint_size);

	private:
		bool IsHexadecimal(char tile_id);
		bool GetIsWallAdjacent(char adjacent_value);

		bool is_iterator_set{ false };
		std::string::iterator blueprint_it;
		std::string m_loaded_blueprint;

		std::array<std::string, 3> blueprints
		{
			"XXXXXXXXXXXXXXXXXXXX"
			"XOOOOOOOOOOOOOOOOOOO"
			"XOOOOOOOOOOOOOOOOOOO"
			"XOXXXXXXOXXOXXXOXXXX"
			"XOOOOOOOOOOOOOOOOOOX"
			"XOOOOOOOOOOOOOOOOOOX"
			"XOXXOXOXXXXXXXXXXOOX"
			"XOOOOOOXOOOOOOOOOOOX"
			"XOOOOOOXOOOOOOOOOOOX"
			"XOOOOOXXOOOOOXOXXOOX"
			"XXXXXXXOOOOXOXOXXOOX"
			"XXXXXOOOOOOXOXOOOOOX"
			"XXXOOOOOOXOXOXOOOOOX"
			"XXXOOOOXOXOXOXXXXOOX"
			"OOOOXOOOOOOOOOOOXOOX"
			"OOOOXOOOOOOOOOOOXOOX"
			"XXOOXXXXOXXOXXXOXOOX"
			"XOOOOOOO1OO2OOO3OOOX"
			"XO!OOOOOOOOOOOOOOOOX"
			"XXXXXXXXXXXXXXXXXXXX",

			"2", "3"
		};

	};
};