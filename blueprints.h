#pragma once

#include <string>
#include <array>


namespace Tmpl8
{
	class Blueprints
	{
	public:
		Blueprints() {};
		bool LoadBlueprint(int level_id);
		unsigned int ReadBlueprint();
		int GetAutotileId(int center_x, int center_y);

	private:
		bool GetIsWallAdjacent(char adjacent_value);

		bool is_iterator_set{ false };
		std::string::iterator blueprint_it;
		std::string m_loaded_blueprint;

		std::array<std::string, 3> blueprints
		{
			"XXXXXXXXXXXXXWWWWWWW"
			"XOOOOOOOOOOOXWWWXXXX"
			"XOOOOOOOOOOOXWWWXOOO"
			"XOOOOOOOPOOOXXXXXOOO"
			"XOOXXXXXXXOOOOOOOOOX"
			"XOOXXXXXXXOOOOOOOOOX"
			"XOOXOOOOOXOOXXXXXOXX"
			"XOOXOOOOOXOOXWWWXOXX"
			"XOOXOOXOOXOOXWWWXOOX"
			"XOOOOOXOOXOOXWWWXOOX"
			"XOOOOOXOOXOOXWWWXXOX"
			"XXXXXXXOOXOOXWWWXXOX"
			"XXXXXXXOOXOOXWWWXOOX"
			"OOOOOOOOOXOOXXXXXOOX"
			"OOOOOOOOOXOOOOOOOOOX"
			"XXOXXOXXXXOOOOOOOOOX"
			"XXOXXOXXXXOOXXXXXOOX"
			"XOOOOOOOOOOOXWWWXOOX"
			"XOOOOOOOOOOOXWWWXOOX"
			"XXXXXXXXXXXXXWWWXXXX",
			
			"XXXXXXXXXX"
			"XOOOOOOOOX"
			"XOOOOOOOOX"
			"XXXXXXXOOX"
			"XXOXXXXOOX"
			"XOOOOOOOOX"
			"XOPOOOOOOX"
			"XXXXXXXOOX"
			"XXXXXXXOOX"
			"XXXXXXXXXX",


			"c"
		};

	};
};