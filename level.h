#pragma once

#include <vector>

#include "surface.h"
#include "blueprints.h"

#include "collidable.h"
#include "obstacle.h"

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
		Level();		

		void CreateLevel(int level_id);
		void CreateLayers();
		vec2& GetPlayerStartPosition();
		vec2 GetBounds();

		// Player collidables are interactable things. The obstacles.
		// The light pickups.
		std::vector<Collidable*>& GetPlayerCollidables();

		void RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket);
		void RegisterViewportSocket(Socket<ViewportMessage>* viewport_socket);

		void Draw(Surface* screen);

		Surface& GetMapLayer() { return m_map_layer; }
		Surface& GetObstacleLayer() { return m_obstacle_layer; }

	private:
		// METHODS
		void CreateComponents();
		void CreateComponentsFromBlueprint();
		void CreateComponentAtPosition(unsigned int blueprint_code, int x, int y);
		void CreateCollidablesList();
		void SetPlayerStartPosition(int x, int y);
		void CreateObstacle(int x, int y, Pixel hex_type);
		/* Setup autotiles */
		int GetAutotileId(int x, int y);
		//bool GetIsWallAdjacent(Pixel adjacent_value);
		void CleanupAutotileId(int& autotile_id);		
		CollidableType GetCollidableType(int autotile_id, Pixel hex_type);
		int GetFrameId(int autotile_id);
		int GetBitwiseOverlap(int autotile_id);

		// ATTRIBUTES
		Surface m_tilemap_smooth{"assets/tilemap_smooth_64x.png"};
		Surface m_tilemap_rough{ "assets/tilemap_rough.png" };
		char* bg{ "assets/noise_robson_1280.png" }; // credit: https://robson.plus/white-noise-image-generator/	

		std::vector<Obstacle> m_obstacles;
		std::vector<Collidable*> m_player_collidables;

		level_blueprint m_current_level_blueprint{ 0, 0, 10, 10 };
		int m_level_id{ 0 };
		vec2 m_player_start_position{ 0.0f, 0.0f };

		Socket<CollisionMessage>* m_collision_socket{ nullptr };
		// Socket<ViewportMessage>* m_viewport_socket{ nullptr };
		// Socket<GlowMessage>* m_glow_socket{ nullptr };

		Surface m_background_layer;
		Surface m_obstacle_layer;
		Surface m_map_layer;

		Blueprints m_blueprints;
	};
};