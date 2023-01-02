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
	constexpr unsigned int MESSAGE_HEX{ 0xFFFFFF00 };
	constexpr unsigned int EOF_HEX{ 0x00000194 };
	constexpr int TILE_SIZE{ 64 };	
	constexpr int AUTOTILE_MAP_FRAME_COUNT{ 47 };
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
	Level::Level(TextRepo& text_repo) :
		m_text_repo{ &text_repo },
		m_background_layer{ bg }, // should change each level.
		m_obstacle_layer{ 1, 1 },
		m_map_layer{ 1, 1 },
		m_revealed_layer{ 1, 1 }
	{	}
			

	// Make level_manager components and images.
	void Level::CreateLevel(int level_id)
	{
		ResetLevel();

		m_level_id = level_id;

		CreateComponents();
		CreateCollidablesList();
		CreateMessageBoxes();
		CreateLayers();
	}
	
	
	void Level::ResetLevel()
	{
		m_player_collidables.clear();
		m_finish_lines.clear();
		m_obstacles.clear();

		m_message_boxes.clear();
	}


	void Level::CreateLayers()
	{
		// Copy bg to the map layer and draw obstacles on top.
		// Used as source for full and temp glow orbs.
		PrepareMapLayer();
				
		// Clear to 0-alpha black and draw obstacles on top.
		// Used as source for safe glow orbs.		
		PrepareObstacleLayer();	

		// Clear to 0-alpha black and draw text boxes on top.
		// Used as source for player glow orb.
		PrepareRevealedLayer();
	}


	void Level::PrepareMapLayer()
	{
		// Set background.
		//Surface background_layer{ bg };
		m_background_layer.Bar(0, 0, m_background_layer.GetWidth(), m_background_layer.GetHeight(), 0xFF000000, true, 0.5f);
		m_background_layer.CopyTo(&m_map_layer, 0, 0);

		//m_map_layer.Clear(0xFFFFFFFF);

		// Add obstacles.
		for (Obstacle& obstacle : m_obstacles)
		{
			obstacle.Draw(&m_map_layer);
		}

		// Add message boxes.
		for (MessageBox& message_box : m_message_boxes)
		{
			if (message_box.m_message_type == MessageType::GUIDE)
			{
				message_box.Draw(&m_map_layer, 2);
			}
		}
	}


	void Level::PrepareObstacleLayer()
	{	
		for (Obstacle& obstacle : m_obstacles)
		{
			if (!(obstacle.m_is_revealed))
			{
				obstacle.Draw(&m_obstacle_layer);
				obstacle.ApplyBitwiseOverlap();
			}
		}
	}


	void Level::PrepareRevealedLayer()
	{
		for (Obstacle& obstacle : m_obstacles)
		{			
			if (obstacle.m_is_revealed)
			{
				obstacle.Draw(&m_revealed_layer);
				obstacle.ApplyBitwiseOverlap();
			}
		}

		int counter = 0;
		for (MessageBox& message_box : m_message_boxes)
		{
			if (message_box.m_message_type == MessageType::TUTORIAL)
			{
				message_box.Draw(&m_revealed_layer, 2.0f);
				++counter;
			}
		}
	}

	Surface& Level::GetMapLayer() 
	{ 
		return m_map_layer;
	}
	
	
	Surface& Level::GetObstacleLayer() 
	{ 
		return m_obstacle_layer;
	}
	

	Surface& Level::GetRevealedLayer() 
	{
		return m_revealed_layer;
	}


	vec2 Level::GetBounds()
	{
		return vec2{ 1.0f * m_map_layer.GetWidth(), 1.0f * m_map_layer.GetHeight() };
	}


	std::vector<Collidable*>& Level::GetPlayerCollidables()
	{
		return m_player_collidables;
	}


	vec2& Level::GetPlayerStartPosition()
	{
		return m_player_start_position;
	}
		

	void Level::RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket)
	{
		m_collision_socket = collision_socket;
	}

	
	void Level::RegisterGlowSocket(Socket<GlowMessage>* glow_socket)
	{
		m_glow_socket = glow_socket;
	}


	void Level::CreateComponents()
	{
		if (m_blueprints.LoadBlueprintData(m_level_id))
		{
			m_blueprint_width = m_blueprints.GetBlueprintWidth();
			m_blueprint_height = m_blueprints.GetBlueprintHeight();

			ResizeSurfaces();

			CreateComponentsFromBlueprint();
		}
	}


	void Level::ResizeSurfaces()
	{
		m_map_layer = Surface{ m_blueprint_width * TILE_SIZE, m_blueprint_height * TILE_SIZE };
		m_map_layer.Clear(0x00000000);

		m_obstacle_layer = Surface{ m_blueprint_width * TILE_SIZE, m_blueprint_height * TILE_SIZE };
		m_obstacle_layer.Clear(0x00000000);

		m_revealed_layer = Surface{ m_blueprint_width * TILE_SIZE, m_blueprint_height * TILE_SIZE };
		m_revealed_layer.Clear(0x00000000);
	}


	void Level::CreateComponentsFromBlueprint()
	{
		for (int y{ 0 }; y < m_blueprint_height; ++y)
		{
			for (int x{ 0 }; x < m_blueprint_width; ++x)
			{
				BlueprintCode blueprint_code{ m_blueprints.GetNextBlueprintCode() };
				CreateComponentAtPosition(blueprint_code, x, y);
			}
		}
	}


	void Level::CreateComponentAtPosition(BlueprintCode& blueprint_code, int x, int y)
	{
		float safe_tile_orb_radius{ static_cast<float>(sqrt((TILE_SIZE * TILE_SIZE) + (TILE_SIZE * TILE_SIZE))) };
		
		switch (blueprint_code.m_tile_id)
		{
		case OBSTACLE_TILE:		
		case UNREACHABLE_TILE:
			CreateObstacle(x, y, blueprint_code.m_tile_id);
			break;
		case SAFE_TILE:
			//CreatePermanentGlowOrb(x, y, safe_tile_orb_radius);			
			CreateObstacle(x, y, blueprint_code.m_tile_id);
			break;
		case NO_TILE:
			// Don't put anything here.
			break;
		case START_TILE:
			SetPlayerStartPosition(x, y);			
			break;
		case PICKUP_TILE:
			CreatePickup(x, y);
			break;
		case EXIT1_TILE:
			AddExitSign(x, y, "assets/good_exit.png");
			break;
		case EXIT2_TILE:
			AddExitSign(x - 1, y, "assets/bad_exit.png");
			break;
		case FINISH_TILE:
			CreateFinishBlock(x, y);
			break;
		case EOF_HEX:
			// Shouldn't happen in prod, but raise a stink for now.
			throw - 1;
			break;
		case 0:
		default:
			// 0 means the file wasn't loaded or an unknown symbol was in the blueprint.
			// Either way, shouldn't happen in prod, but raise a stink for now.
			throw - 2;
			break;
		}
	}


	void Level::CreateCollidablesList()
	{
		// Add obstacles that can interact with player to collidables list.
		for (Obstacle& obstacle : m_obstacles)
		{
			if (obstacle.m_object_type != CollidableType::UNREACHABLE)
				m_player_collidables.push_back(&obstacle);
		}

		for (Collidable& finish_line : m_finish_lines)
		{
			m_player_collidables.push_back(&finish_line);
		}
	}


	void Level::SetPlayerStartPosition(int x, int y)
	{
		m_player_start_position = vec2{
			static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE)
		};
	}


	void Level::CreateObstacle(int x, int y, int tile_id)
	{
		// Use autotile_id (value based on surrounding walls)
		// to determine collidable type (corner, unreachable, or normal)
		// and frame index of tile_map.
		int autotile_id{ GetAutotileId(x, y) };
		CollidableType type{ GetCollidableType(autotile_id, tile_id) };
		int frame_id{ GetFrameId(autotile_id) };
		int bitwise_overlap{ GetBitwiseOverlap(autotile_id) };

		Surface* tilemap{ &m_tilemap_smooth }; //tile_id == OBSTACLE_TILE ? &m_tilemap_smooth : &m_tilemap_rough
		Obstacle obstacle{ x, y, TILE_SIZE, type, tilemap, AUTOTILE_MAP_FRAME_COUNT, frame_id, bitwise_overlap, tile_id == SAFE_TILE };
				
		m_obstacles.push_back(obstacle);
	}


	int Level::GetAutotileId(int center_x, int center_y)
	{
		int autotile_id = m_blueprints.GetAutotileId(center_x, center_y);
		CleanupAutotileId(autotile_id);
		return autotile_id;
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
	

	CollidableType Level::GetCollidableType(int autotile_id, int tile_id)
	{
		if (autotile_id == 255)
		{
			return CollidableType::UNREACHABLE;
		}
		else
		{
			switch (tile_id)
			{
			case OBSTACLE_TILE:			
				return CollidableType::SMOOTH;				
			case SAFE_TILE:
				return CollidableType::PERM_GLOW;
			}
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


	int Level::GetBitwiseOverlap(int autotile_id)
	{
		// Store T/F per bit for if a neighboring obstacle exists in that direction.

		int bitwise_overlap{ 0 };
		
		if (autotile_id & LEFT)
			bitwise_overlap |= 0x0001;

		if (autotile_id & RIGHT)
			bitwise_overlap |= 0x0010;

		if (autotile_id & TOP)
			bitwise_overlap |= 0x0100;

		if (autotile_id & BOTTOM)
			bitwise_overlap |= 0x1000;

		return bitwise_overlap;
	}


	void Level::CreatePickup(int x, int y)
	{
		/*Pickup pickup{ x, y, TILE_SIZE, CollidabelType::Pickup, m_light_pickup };

		m_obstacles.push_back(obstacle);*/
	}


	void Level::AddExitSign(int x, int y, std::string image_path)
	{
		//Sprite sprite{ new Surface(&image_path[0]), 1, true };
		//sprite.Draw(&m_revealed_layer, x * TILE_SIZE, y * TILE_SIZE);

		Surface surface{ &image_path[0] };
		surface.CopyTo(&m_revealed_layer, x * TILE_SIZE, y * TILE_SIZE);
	}


	void Level::CreateFinishBlock(int x, int y)
	{
		Collidable finish_line{ x, y, TILE_SIZE, CollidableType::FINISH_LINE };

		m_finish_lines.push_back(finish_line);
	}


	void Level::CreateMessageBoxes()
	{
		MessageType message_type{ m_level_id == 0 ? MessageType::TUTORIAL : MessageType::GUIDE };

		std::vector<Entry> entries{ m_text_repo->GetEntriesForLevel(m_level_id) };
		
		for (Entry& entry : entries)
		{
			//std::unique_ptr<Sprite> text_sprite{ m_tsmaker.GetTextSprite(entry.m_span, entry.m_text, 0xFFAAAAAA) };
			//m_message_boxes.emplace_back(message_type, std::move(text_sprite), entry.m_position, TILE_SIZE);
			m_message_boxes.emplace_back(message_type, entry, TILE_SIZE, 0xFFAAAAAA);
		}
	}
};