#pragma once

#include <vector>

#include "key_state.h"
#include "collidable.h"
#include "obstacle.h"
#include "player.h"

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
		Level(Surface* screen);		
		void CreateLevel(int level_id);
		void Update(float deltaTime, keyState& leftKey, keyState& rightKey, keyState& upKey, keyState& downKey);
		void Draw();
		void CheckForCollisions();

		void CollisionLoop();

	private:
		// METHODS
		void CreateComponents();
		void CreatePlayer(int x, int y);
		void CreateObstacle(int x, int y, Pixel hex_type);
		/* Setup autotiles */
		int GetAutotileId(int x, int y);
		bool GetIsWallAdjacent(Pixel adjacent_value);
		void CleanupAutotileId(int& autotile_id);		
		CollidableType GetCollidableType(int autotile_id, Pixel hex_type);
		int GetFrameId(int autotile_id);
		/* Collision checking process. */
		std::vector<std::vector<Collidable*>> GetXAxisOverlaps();
		void SortCollidablesOnXAxis();
		std::vector<std::vector<Collidable*>> GetCollisions(std::vector<std::vector<Collidable*>>& x_overlaps);
		void NotifyCollisionPairs(std::vector<std::vector<Collidable*>>& collisions);

		

		// ATTRIBUTES
		Surface* m_screen{ nullptr };
		Surface m_blueprints{ "assets/level-t.png" };
		Surface m_background{ "assets/noise-robson.png" }; // credit: https://robson.plus/white-noise-image-generator/	
		Surface m_tilemap_smooth{"assets/tilemap_smooth_64x.png"};
		Surface m_tilemap_rough{ "assets/tilemap_rough.png" };
		Player m_player;
		std::vector<Collidable*> collidables;
		std::vector<Obstacle> obstacles;
		level_blueprint m_current_level_blueprint{ 0, 0, 10, 10 };
		int m_level_id{ 0 };
	};
};

