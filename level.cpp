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
	
	
	// Constructor.
	Level::Level(TextRepo& text_repo) :
		m_text_repo{ &text_repo },
		m_obstacle_layer{ ScreenWidth, ScreenHeight },
		m_map_layer{ ScreenWidth, ScreenHeight },
		m_revealed_layer{ ScreenWidth, ScreenHeight }
	{	}
			

	// Make components and images.
	void Level::CreateLevel(int level_id)
	{
		ResetLevel();

		m_level_id = level_id;

		if (level_id >= 0)
		{
			CreateComponents();
			CreateCollidableLists();
			SendCollidableLists();
			CreateMessageBoxes();
			CreateLayers();
		}
		else
		{
			ClearSurfaces();
			//PrepareMapLayer();
			m_title.CopyTo(&m_revealed_layer, 494, 355);
		}
	}
	
	
	void Level::ResetLevel()
	{
		m_message_boxes.clear();
		m_obstacles.clear();
		m_moving_obstacles.clear();
		m_finish_lines.clear();
		m_pickups.clear();

		m_obstacle_collidables.clear();
		m_finish_line_collidables.clear();
		m_pickup_collidables.clear();

		m_is_fading_out = false;
		m_opacity = 1.0f;
	}


	void Level::SetPlayerTitlePosition()
	{
		m_player_start_position = vec2{
			static_cast<float>(ScreenWidth / 2),
			static_cast<float>(ScreenHeight / 2)
		};
	}


	void Level::Update(float deltaTime)
	{
		if (m_is_fading_out)
		{
			m_opacity -= opacity_delta * deltaTime;
			opacity_delta += opacity_delta_delta * deltaTime;

			m_opacity = Max(0.0f, m_opacity);
			m_is_fading_out = (m_opacity > 0.0f);
		}

		for (LightPickup& pickup : m_pickups)
		{
			if (pickup.m_is_active)
			{
				pickup.Update(deltaTime, m_opacity);
			}
		}

		for (MovingObstacle& moving_obstacle : m_moving_obstacles)
		{
			moving_obstacle.Update(deltaTime);
		}
	}


	void Level::FadeToBlack()
	{
		m_is_fading_out = true;
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
		return vec2{ 
			static_cast<float>(m_map_layer.GetWidth()),
			static_cast<float>(m_map_layer.GetHeight())
		};
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
			ClearSurfaces();

			CreateComponentsFromBlueprint();
		}
		else
		{			
			ClearSurfaces();
			//CreateTitleObstacle();
		}
	}


	void Level::ResizeSurfaces()
	{
		m_map_layer = Surface{ m_blueprint_width * TILE_SIZE, m_blueprint_height * TILE_SIZE };
		m_obstacle_layer = Surface{ m_blueprint_width * TILE_SIZE, m_blueprint_height * TILE_SIZE };
		m_revealed_layer = Surface{ m_blueprint_width * TILE_SIZE, m_blueprint_height * TILE_SIZE };
	}


	void Level::ClearSurfaces()
	{
		m_map_layer.Clear(0x00000000);
		m_obstacle_layer.Clear(0x00000000);
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
		case HIDDEN_OBSTACLE_TILE:		
		case VISIBLE_OBSTACLE_TILE:
		case UNREACHABLE_OBSTACLE_TILE:
		case DANGEROUS_OBSTACLE_TILE:
		case VISIBLE_MOVING_OBSTACLE_TILE:
		case HIDDEN_MOVING_OBSTACLE_TILE:
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
		case EASY_EXIT_TILE:
			AddExitSign(x, y, "assets/good_exit.png");
			break;
		case HARD_EXIT_TILE:
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


	void Level::SetPlayerStartPosition(int x, int y)
	{
		m_player_start_position = vec2{
			static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE)
		};
	}


	void Level::CreateObstacle(int x, int y, int tile_id)
	{
		switch (tile_id)
		{
		case VISIBLE_MOVING_OBSTACLE_TILE:
		case HIDDEN_MOVING_OBSTACLE_TILE:
		{			
			m_moving_obstacles.emplace_back(GetMoveDirection(x, y), x, y, TILE_SIZE, GetAutotileId(x, y), tile_id, GetTilemapSprite(tile_id), m_obstacle_layer, m_map_layer);
		}
			break;
		default:
		{
			//Obstacle obstacle{ x, y, TILE_SIZE, GetAutotileId(x, y), tile_id, GetTilemapSprite(tile_id) };
			//m_obstacles.push_back(obstacle);
			m_obstacles.emplace_back(x, y, TILE_SIZE, GetAutotileId(x, y), tile_id, GetTilemapSprite(tile_id));
		}
			break;
		}
	}


	std::pair<MoveDirection, int> Level::GetMoveDirection(int x, int y)
	{
		int direction_id = m_blueprints.GetDirectionId(x, y);

		MoveDirection direction{ direction_id < 0 ? MoveDirection::VERTICAL : MoveDirection::HORIZONTAL };
		int tile_span{ abs(direction_id) };

		return std::pair<MoveDirection, int>(direction, tile_span);		
	}


	int Level::GetAutotileId(int x, int y)
	{
		int autotile_id = m_blueprints.GetAutotileId(x, y);
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
	

	Sprite& Level::GetTilemapSprite(int tile_id)
	{
		switch (tile_id)
		{
		case VISIBLE_OBSTACLE_TILE:
		case HIDDEN_OBSTACLE_TILE:
		case UNREACHABLE_OBSTACLE_TILE:
		case VISIBLE_MOVING_OBSTACLE_TILE:
		case HIDDEN_MOVING_OBSTACLE_TILE:
			return m_normal_obstacle_tilemap_sprite;
		case DANGEROUS_OBSTACLE_TILE:
			return m_dangerous_obstacle_tilemap_sprite;
		default:
			return m_normal_obstacle_tilemap_sprite;
		}
	}


	void Level::CreatePickup(int x, int y)
	{		
		LightPickup pickup{ x, y, TILE_SIZE, CollidableType::PICKUP, m_light_pickup, m_glow_socket };

		m_pickups.push_back(pickup);
	}


	void Level::AddExitSign(int x, int y, std::string image_path)
	{
		Surface surface{ &image_path[0] };
		surface.CopyTo(&m_revealed_layer, x * TILE_SIZE, y * TILE_SIZE);
	}


	void Level::CreateFinishBlock(int x, int y)
	{
		FinishLine finish_line{ x, y, TILE_SIZE, CollidableType::FINISH_LINE };

		m_finish_lines.push_back(finish_line);
	}


	void Level::CreateCollidableLists()
	{
		// Add obstacles that can interact with player to collidables list.
		for (Obstacle& obstacle : m_obstacles)
		{
			if (obstacle.m_collidable_type != CollidableType::OBSTACLE_UNREACHABLE)
				m_obstacle_collidables.push_back(&obstacle);
		}

		for (MovingObstacle& moving_obstacle : m_moving_obstacles)
		{
			m_obstacle_collidables.push_back(&moving_obstacle);
		}

		for (LightPickup& pickup : m_pickups)
		{
			m_pickup_collidables.push_back(&pickup);
		}

		for (Collidable& finish_line : m_finish_lines)
		{
			m_finish_line_collidables.push_back(&finish_line);
		}
	}


	void Level::SendCollidableLists()
	{
		m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_OBSTACLE_LIST, m_obstacle_collidables });
		m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_PICKUP_LIST, m_pickup_collidables });
		m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_FINISH_LINE_LIST, m_finish_line_collidables });
	}


	void Level::CreateMessageBoxes()
	{
		std::vector<Entry> entries{ m_text_repo->GetEntriesForLevel(m_level_id) };

		for (Entry& entry : entries)
		{
			m_message_boxes.emplace_back(entry, TILE_SIZE, 0xFFAAAAAA);
		}
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
		char* asset_path;
		switch (m_level_id)
		{
		case -1:
			asset_path = "assets/backgrounds/title.png"; // Justin Prno at https://dribbble.com/hi_jmp
			break;
		case 0:
			asset_path = "assets/backgrounds/tutorial.png"; // Justin Prno at https://dribbble.com/hi_jmp
			break;
		case 1:
			asset_path = "assets/backgrounds/L01.png"; // Justin Prno at https://dribbble.com/hi_jmp
			break;
		case 2:
			asset_path = "assets/backgrounds/L02.png"; // Justin Prno at https://dribbble.com/hi_jmp
			break;
		default:
			asset_path = "assets/backgrounds/tutorial.png"; // Justin Prno at https://dribbble.com/hi_jmp
			break;
		}

		Surface background_layer{ asset_path };
		background_layer.Bar(0, 0, background_layer.GetWidth(), background_layer.GetHeight(), 0xFF000000, true, 0.5f);
		background_layer.CopyTo(&m_map_layer, 0, 0);


		// Add obstacles.
		for (Obstacle& obstacle : m_obstacles)
		{
			if (obstacle.m_collidable_type != CollidableType::OBSTACLE_MOVING_VISIBLE
				&& obstacle.m_collidable_type != CollidableType::OBSTACLE_MOVING_HIDDEN)
			{
				obstacle.Draw(&m_map_layer);
			}
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
			if (obstacle.m_collidable_type != CollidableType::OBSTACLE_UNREACHABLE
				&& obstacle.m_collidable_type != CollidableType::OBSTACLE_MOVING_VISIBLE
				&& obstacle.m_collidable_type != CollidableType::OBSTACLE_MOVING_HIDDEN)
			{
				obstacle.Draw(&m_obstacle_layer);
				obstacle.ApplyOverlap();
			}
		}
	}


	void Level::PrepareRevealedLayer()
	{
		for (Obstacle& obstacle : m_obstacles)
		{
			if (obstacle.m_collidable_type == CollidableType::OBSTACLE_MOVING_VISIBLE
				|| obstacle.m_collidable_type == CollidableType::OBSTACLE_VISIBLE)
			{
				obstacle.Draw(&m_revealed_layer);
				obstacle.ApplyOverlap();
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
};