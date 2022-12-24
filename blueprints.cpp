#include "blueprints.h"

#include <stdexcept>


namespace Tmpl8
{
	// Constants.
	constexpr unsigned int SMOOTH_HEX{ 0xFF000000 };
	constexpr unsigned int STARTING_HEX{ 0xFF0000FF };
	constexpr unsigned int ROUGH_HEX{ 0xFFFF0000 };
	constexpr unsigned int UNREACHABLE_HEX{ 0xFFFFFF00 };
	constexpr unsigned int OPEN_HEX{ 0xFFFFFFFF };
	constexpr unsigned int EOF_HEX{ 0x00000194 };
	constexpr unsigned int BLUEPRINT_SIZE{ 10 };


	bool Blueprints::LoadBlueprint(int level_id)
	{
		is_iterator_set = false;

		try
		{
			m_loaded_blueprint = blueprints.at(level_id - 1);
			blueprint_it = m_loaded_blueprint.begin();
			is_iterator_set = true;
		}
		catch (std::out_of_range)
		{
			is_iterator_set = false;
		}
		
		return is_iterator_set;
	}


	unsigned int Blueprints::ReadBlueprint()
	{
		if (!is_iterator_set || m_loaded_blueprint.size() == 0) return 0;
		
		// If read full blueprint, return EOF.
		if (blueprint_it == m_loaded_blueprint.end())
		{
			is_iterator_set = false;
			return EOF_HEX;
		}

		// Otherwise get next code.
		unsigned int blueprint_code;
		switch (*blueprint_it)
		{
		case 'X':
			blueprint_code = SMOOTH_HEX;
			break;
		case 'O':
			blueprint_code = OPEN_HEX;
			break;
		case 'W':
			blueprint_code = UNREACHABLE_HEX;
			break;
		case 'P':
			blueprint_code = STARTING_HEX;
			break;
		default:
			blueprint_code = 0;
			break;
		}
			
		// Advance iter.
		++blueprint_it;

		// Return code.
		return blueprint_code;		
	}


	int Blueprints::GetAutotileId(int center_x, int center_y)
	{
		/* Autotile mapping credit: Godot docs (https://docs.godotengine.org/en/stable/tutorials/2d/using_tilemaps.html)
			Loop over the 8 adjacent tiles, starting in the upper-left to the lower-right.
			If the tile is a wall, add the tile's value to the autotile id. Any tile that
			is out of bounds is considered a wall (this prevents wall border lines being drawn
			along the edge of the entire leve - the walls will appear to go beyond the visible level).

			Each tile has a bit value that increases by the power of 2 (b1, b10, b100, etc). This value
			is added to the autotile id if the tile is a wall.
		*/

		if (m_loaded_blueprint.size() == 0) return 0;

		int autotile_id{ 0 }, tile_value{ 1 };

		for (int y{ center_y - 1 }; y <= center_y + 1; y++)
		{
			for (int x{ center_x - 1 }; x <= center_x + 1; x++)
			{
				if (y != center_y || x != center_x) // Skip checking self.
				{
					bool out_of_bounds = (y < 0 || y >= BLUEPRINT_SIZE || x < 0 || x >= BLUEPRINT_SIZE);

					if (out_of_bounds || GetIsWallAdjacent(m_loaded_blueprint[x + (y * BLUEPRINT_SIZE)]))
						autotile_id |= tile_value;

					tile_value <<= 1;
				}
			}
		}
		return autotile_id;
	}

	
	bool Blueprints::GetIsWallAdjacent(char adjacent_value)
	{
		switch (adjacent_value)
		{
		case 'X':
		case 'W':
			return true;
		default:
			return false;
		}
	}
};