#include <algorithm>

#include "template.h"
#include "surface.h"
#include "level.h"


namespace Tmpl8
{
	// Constants.
	constexpr unsigned int SMOOTH_HEX{ 0xFF000000 };
	constexpr unsigned int STARTING_HEX{ 0xFF0000FF };
	constexpr unsigned int ROUGH_HEX{ 0xFFFF0000 };
	constexpr unsigned int UNREACHABLE_HEX{ 0xFFFFFF00 };
	constexpr unsigned int OPEN_HEX{ 0xFFFFFFFF };
	constexpr unsigned int TILE_SIZE{ 64 };
	constexpr unsigned int BLUEPRINT_SIZE{ 10 };
	constexpr unsigned int AUTOTILE_MAP_FRAME_COUNT{ 47 };
	/* Tile positions and bit values. */
	constexpr unsigned int TOP_LEFT = 1;
	constexpr unsigned int TOP = 2;
	constexpr unsigned int TOP_RIGHT = 4;
	constexpr unsigned int LEFT = 8;
	constexpr unsigned int RIGHT = 16;
	constexpr unsigned int BOTTOM_LEFT = 32;
	constexpr unsigned int BOTTOM = 64;
	constexpr unsigned int BOTTOM_RIGHT = 128;
	
	// Constructor.
	Level::Level() :
		m_glow_manager{}
	{
		m_current_level_blueprint.height = BLUEPRINT_SIZE;
		m_current_level_blueprint.width = BLUEPRINT_SIZE;

		m_background.Bar(0, 0, m_background.GetWidth(), m_background.GetHeight(), 0xff000000, true, 0.8f);
	}


	// Make level components and images.
	void Level::CreateLevel(int level_id)		
	{
		m_level_id = level_id;
		m_current_level_blueprint.x = (m_level_id - 1) * BLUEPRINT_SIZE;
		CreateComponents();
	}


	void Level::Draw(Surface* screen)
	{
		//m_background.CopyTo(screen, 0, 0);
		screen->Clear(0);

		// Draw level components. (will mix with glow Surface first).
		for (Obstacle& obstacle : m_obstacles)
		{
			obstacle.Draw(screen);
		}
	}

	void Level::Update()
	{
		m_glow_manager.Update();
	}

	std::vector<GlowOrb>& Level::GetViewportCollidables()
	{
		return m_glow_manager.GetViewportCollidables();
	}


	std::vector<Collidable*>& Level::GetPlayerCollidables()
	{
		return m_player_collidables;
	}


	GlowSocket& Level::GetGlowSocket()
	{
		return m_glow_manager.GetGlowSocket();
	}


	vec2& Level::GetPlayerStartPosition()
	{
		return m_player_start_position;
	}
		
	// Create and place components.
	void Level::CreateComponents()
	{
		// Construct level from blueprint.
		//int x_start = (m_level_id - 1) * blueprint_size;
		Pixel* architect{ m_blueprints.GetBuffer() + m_current_level_blueprint.x };

		for (int y{ 0 }; y < BLUEPRINT_SIZE; y++)
		{
			for (int x{ 0 }; x < BLUEPRINT_SIZE; x++)
			{
				// Find and store obstacle locations.
				switch (architect[x])
				{
				case SMOOTH_HEX:
				case ROUGH_HEX:
				case UNREACHABLE_HEX:
					// Get autotile image id and store obstacle.
					CreateObstacle(x, y, architect[x]);
					break;
				case STARTING_HEX:
					// Set player x,y.
					SetPlayerStartPosition(x, y);
					break;
				default:
					break;
				}
			}
			architect += m_blueprints.GetPitch();
		}

		// Add obstacles that can interact with player to collidables list.
		for (Obstacle& obstacle : m_obstacles)
		{
			if (obstacle.m_object_type != CollidableType::UNREACHABLE)
				m_player_collidables.push_back(&obstacle);
		}
	}


	void Level::SetPlayerStartPosition(int x, int y)
	{
		m_player_start_position = vec2{
			static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE)
		};
	}


	void Level::CreateObstacle(int x, int y, Pixel hex_type)
	{
		// Use autotile_id (value based on surrounding walls)
		// to determine collidable type (corner, unreachable, or normal)
		// and frame index of tile_map.
		int autotile_id{ GetAutotileId(x, y) };
		CollidableType type{ GetCollidableType(autotile_id, hex_type) };		
		int frame_id{ GetFrameId(autotile_id) };

		Surface* tilemap{ hex_type == SMOOTH_HEX ? &m_tilemap_smooth : &m_tilemap_rough };
		Obstacle obstacle{ x, y, TILE_SIZE, type, tilemap, AUTOTILE_MAP_FRAME_COUNT, frame_id };
				
		m_obstacles.push_back(obstacle);
	}


	int Level::GetAutotileId(int center_x, int center_y)
	{
		/* Autotile mapping credit: Godot docs (https://docs.godotengine.org/en/stable/tutorials/2d/using_tilemaps.html)
			Loop over the 8 adjacent tiles, starting in the upper-left to the lower-right.
			If the tile is a wall, add the tile's value to the autotile id. Any tile that
			is out of bounds is considered a wall (this prevents wall border lines being drawn
			along the edge of the entire leve - the walls will appear to go beyond the visible level).
			
			Each tile has a bit value that increases by the power of 2 (b1, b10, b100, etc). This value
			is added to the autotile id if the tile is a wall.
		*/

		Pixel* autotiler{ m_blueprints.GetBuffer() + m_current_level_blueprint.x };		
		int autotile_id{ 0 }, tile_value{ 1 };

		for (int y{ center_y - 1 }; y <= center_y + 1; y++)
		{			
			for (int x{ center_x - 1 }; x <= center_x + 1; x++)
			{	
				if (y != center_y || x != center_x) // Skip checking self.
				{
					bool out_of_bounds = (y < 0 || y >= BLUEPRINT_SIZE || x < 0 || x >= BLUEPRINT_SIZE);

					if (out_of_bounds || GetIsWallAdjacent(autotiler[x + (y * m_blueprints.GetPitch())]))
						autotile_id |= tile_value;

					tile_value <<= 1;
				}				
			}
		}
		CleanupAutotileId(autotile_id);
		return autotile_id;
	}


	bool Level::GetIsWallAdjacent(Pixel adjacent_value)
	{
		switch (adjacent_value)
		{
		case SMOOTH_HEX:
		case ROUGH_HEX:
		case UNREACHABLE_HEX:
			return true;
		default:
			return false;
		}
	}


	void Level::CleanupAutotileId(int& autotile_id)
	{
		/* If a tile detects a corner but no adjacent wall, ignore the corner. */
		
		if (~autotile_id & TOP)
			autotile_id &= ~(TOP_LEFT | TOP_RIGHT);

		if (~autotile_id & LEFT)
			autotile_id &= ~(TOP_LEFT | BOTTOM_LEFT);

		if (~autotile_id & RIGHT)
			autotile_id &= ~(TOP_RIGHT | BOTTOM_RIGHT);

		if (~autotile_id & BOTTOM)
			autotile_id &= ~(BOTTOM_LEFT | BOTTOM_RIGHT);
	}

	CollidableType Level::GetCollidableType(int autotile_id, Pixel hex_type)
	{
		switch (autotile_id)
		{
		// The player cannot collide with this piece.
		// It will only show an image, but not be part of collision detection.
		case 255:	
			return CollidableType::UNREACHABLE;
		
		// These are all "outie" corner pieces.
		// If the player hits one, they will ricochet off at an angle.
		case 208:
		case 104: 
		case 80:
		case 72:
		case 22:
		case 11:
		case 2:
		case 16:
		case 8:
		case 0:
		case 18:
		case 10:
			if (hex_type == SMOOTH_HEX) //testing
				return CollidableType::SMOOTH;
				//return CollidableType::SMOOTH_CORNER;
			else
				return CollidableType::ROUGH;
				//return CollidableType::ROUGH_CORNER;		
		
		// The rest are regular wall pieces.
		// The player will bounce against them normally.
		default:
			if (hex_type == SMOOTH_HEX)
				return CollidableType::SMOOTH;
			else
				return CollidableType::ROUGH;
		}
	}
	

	int Level::GetFrameId(int autotile_id)
	{
		switch (autotile_id)
		{		
		case 208:	return 0;
		case 248:	return 1;
		case 104:	return 2;
		case 64:	return 3;
		case 80:	return 4;
		case 216:	return 5;
		case 120:	return 6;
		case 72:	return 7;
		case 214:	return 8;
		case 255:	return 9;
		case 107:	return 10;
		case 66:	return 11;
		case 210:	return 12;
		case 254:	return 13;
		case 251:	return 14;
		case 106:	return 15;
		case 22:	return 16;
		case 31:	return 17;
		case 11:	return 18;
		case 2:		return 19;
		case 86:	return 20;
		case 223:	return 21;
		case 127:	return 22;
		case 75:	return 23;
		case 16:	return 24;
		case 24:	return 25;
		case 8:		return 26;
		case 0:		return 27; // default return;
		case 18:	return 28;
		case 30:	return 29;
		case 27:	return 30;
		case 10:	return 31;
		case 88:	return 32;
		case 74:	return 33;
		case 250:	return 34;
		case 123:	return 35;
		case 218:	return 36;
		case 122:	return 37;
		case 126:	return 38;
		case 219:	return 39;
		case 82:	return 40;
		case 26:	return 41;
		case 222:	return 42;
		case 95:	return 43;
		case 94:	return 44;
		case 91:	return 45;
		case 90:	return 46;
		default:	return 27; // isolated wall (case 0).
		}
	}
};