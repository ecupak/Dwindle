#pragma once

#include <vector>

#include "surface.h"
#include "blueprints.h"

#include "text_repo.h"
#include "message_box.h"

#include "collidable.h"
#include "obstacle.h"
#include "moving_obstacle.h"
#include "obstacle_consts.h"

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
		void Draw() {};

		void CreateLevel(int level_id);
		
		vec2& GetPlayerStartPosition();
		vec2 GetBounds();

		void RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket);
		void RegisterGlowSocket(Socket<GlowMessage>* glow_socket);
		
		Surface& GetMapLayer();
		Surface& GetObstacleLayer();
		Surface& GetRevealedLayer();

		void FadeToBlack();

	private:
		// METHODS
		void CreateLayers();
		void ResetLevel();
		void ResizeSurfaces();
		void ClearSurfaces();
		void SetPlayerTitlePosition();

		void CreateComponents();
		void CreateComponentsFromBlueprint();
		void CreateComponentAtPosition(BlueprintCode& blueprint_code, int x, int y);
		void CreateCollidableLists();
		void SendCollidableLists();

		std::pair<MoveDirection, int> GetMoveDirection(int x, int y);
		int GetAutotileId(int x, int y);
		void CleanupAutotileId(int& autotile_id);
		Sprite& GetTilemapSprite(int tile_id);

		void SetPlayerStartPosition(int x, int y);
		void CreateObstacle(int x, int y, int tile_id);
		void CreateMessageBoxes();
		void AddExitSign(int x, int y, std::string image_path);
		void CreateFinishBlock(int x, int y);
		void CreatePickup(int x, int y);

		CollidableType GetCollidableType(int autotile_id, int tile_id);
		int GetFrameId(int autotile_id);
		int GetBitwiseOverlap(int autotile_id);

		void PrepareMapLayer();
		void PrepareObstacleLayer();
		void PrepareRevealedLayer();

		// ATTRIBUTES
		Surface m_light_pickup{ "assets/light-lg.png" };
		Surface m_title{ "assets/title.png" };

		std::vector<Obstacle> m_obstacles;
		std::vector<MovingObstacle> m_moving_obstacles;
		std::vector<FinishLine> m_finish_lines;
		std::vector<LightPickup> m_pickups;
		std::vector<Collidable*> m_obstacle_collidables;
		std::vector<Collidable*> m_finish_line_collidables;
		std::vector<Collidable*> m_pickup_collidables;
		std::vector<MessageBox> m_message_boxes;
				
		int m_level_id{ 0 };
		int m_blueprint_width{ 0 };
		int m_blueprint_height{ 0 };

		vec2 m_player_start_position{ ScreenWidth / 2.0f, ScreenHeight / 2.0f };

		Socket<CollisionMessage>* m_collision_socket{ nullptr };
		Socket<GlowMessage>* m_glow_socket{ nullptr };

		//Surface m_background_layer;
		Surface m_obstacle_layer;
		Surface m_map_layer;
		Surface m_revealed_layer;

		Sprite m_normal_obstacle_tilemap_sprite{ new Surface("assets/tilemaps/tilemap_normal_64x.png"), 47, true };
		Sprite m_dangerous_obstacle_tilemap_sprite{ new Surface("assets/tilemaps/tilemap_dangerous_64x.png"), 47, true };

		Blueprints m_blueprints;
		TextRepo* m_text_repo{ nullptr };

		bool m_is_fading_out{ false };
		float m_opacity{ 1.0f };
		float opacity_delta{ 0.0f };
		float opacity_delta_delta{ 0.2f };
	};
};