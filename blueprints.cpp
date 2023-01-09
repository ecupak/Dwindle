#include "blueprints.h"

#include <stdexcept>


namespace Tmpl8
{
	bool Blueprints::LoadBlueprintData(int level_id)
	{
		is_iterator_set = false;

		try
		{
			m_loaded_blueprint_data = blueprint_data.at(level_id);
			blueprint_it = m_loaded_blueprint_data.m_blueprint.begin();
			is_iterator_set = true;
		}
		catch (std::out_of_range)
		{
			is_iterator_set = false;
		}
		
		return (is_iterator_set && m_loaded_blueprint_data.m_blueprint.size() > 0);
	}


	int Blueprints::GetBlueprintWidth()
	{
		return m_loaded_blueprint_data.m_width;
	}


	int Blueprints::GetBlueprintHeight()
	{
		return m_loaded_blueprint_data.m_height;
	}


	BlueprintCode Blueprints::GetNextBlueprintCode()
	{
		BlueprintCode blueprint_code;

		if (!is_iterator_set)
		{
			blueprint_code.m_tile_id = UNKNOWN_MARK;
			return blueprint_code;
		}

		// If read full blueprint, return EOF.
		if (blueprint_it == m_loaded_blueprint_data.m_blueprint.end())
		{
			is_iterator_set = false;
			blueprint_code.m_tile_id = EOF_MARK;
			return blueprint_code;
		}

		// Otherwise get next code.		
		switch (*blueprint_it)
		{
		case NO_TILE:
		case HIDDEN_OBSTACLE_TILE:		
		case VISIBLE_OBSTACLE_TILE:
		case UNREACHABLE_OBSTACLE_TILE:
		case DANGEROUS_OBSTACLE_TILE:
		case VISIBLE_MOVING_OBSTACLE_TILE:
		case HIDDEN_MOVING_OBSTACLE_TILE:
		case START_TILE:
		case PICKUP_TILE:
		case EASY_EXIT_TILE:
		case HARD_EXIT_TILE:
		case FINISH_TILE:
			blueprint_code.m_tile_id = *blueprint_it;
			break;
		// Numbers for moving obstacle.
		case 49:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
			blueprint_code.m_tile_id = NO_TILE;
			break;
		default:			
			blueprint_code.m_tile_id = UNKNOWN_MARK;
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
			along the edge of the entire leve - the walls will appear to go beyond the visible level_manager).

			Each tile has a bit value that increases by the power of 2 (b1, b10, b100, etc). This value
			is added to the autotile id if the tile is a wall.
		*/

		if (m_loaded_blueprint_data.m_blueprint.size() == 0) return 0;

		int autotile_id{ 0 }, tile_value{ 1 };

		for (int y{ center_y - 1 }; y <= center_y + 1; y++)
		{
			for (int x{ center_x - 1 }; x <= center_x + 1; x++)
			{
				if (y != center_y || x != center_x) // Skip checking self.
				{
					bool out_of_bounds = (y < 0 || y >= m_loaded_blueprint_data.m_height || x < 0 || x >= m_loaded_blueprint_data.m_width);

					if (out_of_bounds || GetIsWallAdjacent(m_loaded_blueprint_data.m_blueprint[x + (y * m_loaded_blueprint_data.m_width)]))
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
		case HIDDEN_OBSTACLE_TILE:
		case VISIBLE_OBSTACLE_TILE:
		case UNREACHABLE_OBSTACLE_TILE:
		case DANGEROUS_OBSTACLE_TILE:
			return true;
		default:
			return false;
		}
	}


	int Blueprints::GetDirectionId(int center_x, int center_y)
	{
		/* Autotile mapping credit: Godot docs (https://docs.godotengine.org/en/stable/tutorials/2d/using_tilemaps.html)
			Loop over the 8 adjacent tiles, starting in the upper-left to the lower-right.
			If the tile is a wall, add the tile's value to the autotile id. Any tile that
			is out of bounds is considered a wall (this prevents wall border lines being drawn
			along the edge of the entire leve - the walls will appear to go beyond the visible level_manager).

			Each tile has a bit value that increases by the power of 2 (b1, b10, b100, etc). This value
			is added to the autotile id if the tile is a wall.
		*/

		if (m_loaded_blueprint_data.m_blueprint.size() == 0) return 0;

		int direction_id{ 0 };

		for (int y{ center_y - 1 }; y <= center_y + 1; y++)
		{
			for (int x{ center_x - 1 }; x <= center_x + 1; x++)
			{
				if (y != center_y || x != center_x) // Skip checking self.
				{
					bool out_of_bounds = (y < 0 || y >= m_loaded_blueprint_data.m_height || x < 0 || x >= m_loaded_blueprint_data.m_width);

					if (!out_of_bounds && HasDirectionInfo(direction_id, x, y, center_x))
						return direction_id;
				}
			}
		}
		return 0;
	}

	
	

	bool Blueprints::HasDirectionInfo(int& direction_id, int x, int y, int center_x)
	{
		char id = m_loaded_blueprint_data.m_blueprint[x + (y * m_loaded_blueprint_data.m_width)];

		// char 49 is number 1 (char 57 is number 9).
		if (id >= 49 && id <= 57)
		{
			id -= 48; // Convert to range 1 to 9;

			if (x < center_x)
			{
				id *= -1; // Negative value denotes initial move direction to the left.
			}

			direction_id = id;

			return true;
		}

		return false;
	}
};