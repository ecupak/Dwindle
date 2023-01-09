#pragma once

#include "obstacle.h"

#include <memory>


namespace Tmpl8
{
	enum class MoveDirection
	{
		LEFT,
		RIGHT,
		NONE,
	};


	class MovingObstacle : public Obstacle
	{
	public:
		MovingObstacle(std::pair<MoveDirection, int>direction_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite, Surface& obstacle_layer, Surface& map_layer);

		void Update(float deltaTime);
		//void Draw(Surface* screen) override;
		void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity = 1.0f) override;

	private:
		CollidableType GetCollidableType(int tile_id);
		int GetDrawOrder(int tile_id);
		void UpdatePosition();

		void DrawOntoObstacleLayer();
		void DrawOntoMapLayer();
		void StoreLastDrawnCoordinates();

		void RestoreMapLayer();
		void CopyMapLayer();

		// Movement plan.
		int m_sign_of_travel_direction{ 0 };
		float m_travel_distance{ 0 };
		float m_min_offset{ 0.0f };
		float m_max_offset{ 0.0f };
		// Precalculated for collision box math.
		int m_half_size{ 32 };
		// For drawing.
		Surface& m_obstacle_layer;
		Surface& m_map_layer;
		Surface m_clipboard;
		int m_last_drawn_left{ 0 }, m_last_drawn_right{ 0 }, m_last_drawn_top{ 0 }, m_last_drawn_bottom{ 0 };

		vec2 m_offset{ 0.0f, 0.0f };
		vec2 m_prev_offset{ 0.0f, 0.0f };
		int m_prev_left{ 0 }, m_prev_top{ 0 }, m_prev_right{ 0 }, m_prev_bottom{ 0 };
		
		float m_speed{ 80.0f };		
		float m_elapsed_time{ 0.0f };
		bool m_is_clipboard_ready{ false };
	};

};

