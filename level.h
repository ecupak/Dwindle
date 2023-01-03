#pragma once

#include <vector>

#include "surface.h"
#include "blueprints.h"

#include "text_repo.h"
#include "message_box.h"

#include "collidable.h"
#include "obstacle.h"
#include "light_pickup.h"
#include "finish_line.h"

#include "glow_socket.h"
#include "collision_socket.h"
#include "viewport_socket.h"


namespace Tmpl8
{
	struct level_blueprint
	{
		int x, y;
		int width, height;
	};

	class Level
	{
	public:
		Level(TextRepo& text_repo);

		void Update(float deltaTime);
		void Draw();

		void CreateLevel(int level_id);
		
		vec2& GetPlayerStartPosition();
		vec2 GetBounds();

		void RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket);
		void RegisterGlowSocket(Socket<GlowMessage>* glow_socket);
		//void RegisterViewportSocket(Socket<ViewportMessage>* viewport_socket);
		
		Surface& GetMapLayer();
		Surface& GetObstacleLayer();
		Surface& GetRevealedLayer();

	private:
		// METHODS
		void CreateLayers();
		void ResetLevel();
		void ResizeSurfaces();

		void CreateComponents();
		void CreateComponentsFromBlueprint();
		void CreateComponentAtPosition(BlueprintCode& blueprint_code, int x, int y);
		void CreateCollidableLists();
		void SendCollidableLists();

		void SetPlayerStartPosition(int x, int y);
		void CreateObstacle(int x, int y, int tile_id);
		void CreateMessageBoxes();
		void AddExitSign(int x, int y, std::string image_path);
		void CreateFinishBlock(int x, int y);
		void CreatePickup(int x, int y);

		/* Setup autotiles */
		int GetAutotileId(int x, int y);
		//bool GetIsWallAdjacent(Pixel adjacent_value);
		void CleanupAutotileId(int& autotile_id);		
		CollidableType GetCollidableType(int autotile_id, int tile_id);
		int GetFrameId(int autotile_id);
		int GetBitwiseOverlap(int autotile_id);

		void PrepareMapLayer();
		void PrepareObstacleLayer();
		void PrepareRevealedLayer();

		// ATTRIBUTES
		Surface m_tilemap_smooth{"assets/tilemap_smooth_64x.png"};
		Surface m_tilemap_rough{ "assets/tilemap_rough.png" };		
		Surface m_light_pickup{ "assets/light.png" };
		char* bg{ "assets/noise_robson_1280.png" }; // credit: https://robson.plus/white-noise-image-generator/	

		std::vector<Obstacle> m_obstacles;		
		std::vector<FinishLine> m_finish_lines;
		std::vector<LightPickup> m_pickups;
		std::vector<Collidable*> m_obstacle_collidables;
		std::vector<Collidable*> m_finish_line_collidables;
		std::vector<Collidable*> m_pickup_collidables;
		std::vector<MessageBox> m_message_boxes;

		//level_blueprint m_current_level_blueprint{ 0, 0, 10, 10 };
		
		int m_level_id{ 0 };
		int m_blueprint_width{ 0 };
		int m_blueprint_height{ 0 };

		vec2 m_player_start_position{ 0.0f, 0.0f };

		Socket<CollisionMessage>* m_collision_socket{ nullptr };
		// Socket<ViewportMessage>* m_viewport_socket{ nullptr };
		Socket<GlowMessage>* m_glow_socket{ nullptr };

		Surface m_background_layer;
		Surface m_obstacle_layer;
		Surface m_map_layer;
		Surface m_revealed_layer;

		Blueprints m_blueprints;
		TextRepo* m_text_repo{ nullptr };
	};
};