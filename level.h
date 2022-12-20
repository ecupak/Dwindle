#pragma once

#include <vector>
#include <forward_list>

#include "surface.h"
#include "key_state.h"
#include "collidable.h"
#include "obstacle.h"
#include "player.h"

#include "glow_manager.h"

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
		void Draw(Surface* screen);

		vec2& GetPlayerStartPosition();

		// Viewport collidables are visual things. The glow orbs on the map.
		// The glow of the light pickups.
		std::vector<GlowOrb>& GetViewportCollidables();

		// Player collidables are interactable things. The obstacles.
		// The light pickups.
		std::vector<Collidable*>& GetPlayerCollidables();


		GlowSocket& GetGlowSocket();
		void Update();

	private:
		// METHODS
		void CreateComponents();
		void SetPlayerStartPosition(int x, int y);
		void CreateObstacle(int x, int y, Pixel hex_type);
		/* Setup autotiles */
		int GetAutotileId(int x, int y);
		bool GetIsWallAdjacent(Pixel adjacent_value);
		void CleanupAutotileId(int& autotile_id);		
		CollidableType GetCollidableType(int autotile_id, Pixel hex_type);
		int GetFrameId(int autotile_id);
		
		// ATTRIBUTES
		Surface m_blueprints{ "assets/level-t.png" };
		Surface m_background{ "assets/noise-robson.png" }; // credit: https://robson.plus/white-noise-image-generator/	
		Surface m_tilemap_smooth{"assets/tilemap_smooth_64x.png"};
		Surface m_tilemap_rough{ "assets/tilemap_rough.png" };		

		std::vector<Obstacle> m_obstacles;
		std::vector<Collidable*> m_player_collidables;

		level_blueprint m_current_level_blueprint{ 0, 0, 10, 10 };
		int m_level_id{ 0 };
		vec2 m_player_start_position{ 0.0f, 0.0f };

		GlowManager m_glow_manager;
	};
};

