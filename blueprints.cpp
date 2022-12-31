#include "blueprints.h"

#include <stdexcept>


namespace Tmpl8
{
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


	BlueprintCode Blueprints::GetNextBlueprintCode()
	{
		BlueprintCode blueprint_code;

		if (!is_iterator_set || m_loaded_blueprint.size() == 0)
		{
			blueprint_code.m_tile_id = UNKNOWN_MARK;
			return blueprint_code;
		}

		// If read full blueprint, return EOF.
		if (blueprint_it == m_loaded_blueprint.end())
		{
			is_iterator_set = false;
			blueprint_code.m_tile_id = EOF_MARK;
			return blueprint_code;
		}

		// Otherwise get next code.		
		switch (*blueprint_it)
		{
		case OBSTACLE_TILE:
		case NO_TILE:
		case UNREACHABLE_TILE:
		case START_TILE:
			blueprint_code.m_tile_id = *blueprint_it;
			break;
		default:
			if (IsHexadecimal(*blueprint_it))
			{
				blueprint_code.m_tile_id = MESSAGE_TILE;
				blueprint_code.SetTileDetail(*blueprint_it);
			}
			else
			{
				blueprint_code.m_tile_id = UNKNOWN_MARK;
			}
			break;
		}		
			
		// Advance iter.
		++blueprint_it;

		// Return code.
		return blueprint_code;		
	}


	bool Blueprints::IsHexadecimal(char tile_id)
	{
		return (
			(tile_id >= 48 && tile_id <= 57)	// ASCII 0 to 9.
			|| (tile_id >= 65 && tile_id <= 70) // ASCII A to F.
		);
	}


	int Blueprints::GetAutotileId(int center_x, int center_y, int blueprint_size)
	{
		/* Autotile mapping credit: Godot docs (https://docs.godotengine.org/en/stable/tutorials/2d/using_tilemaps.html)
			Loop over the 8 adjacent tiles, starting in the upper-left to the lower-right.
			If the tile is a wall, add the tile's value to the autotile id. Any tile that
			is out of bounds is considered a wall (this prevents wall border lines being drawn
			along the edge of the entire leve - the walls will appear to go beyond the visible level_manager).

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
					bool out_of_bounds = (y < 0 || y >= blueprint_size || x < 0 || x >= blueprint_size);

					if (out_of_bounds || GetIsWallAdjacent(m_loaded_blueprint[x + (y * blueprint_size)]))
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