#pragma once

#include "obstacle.h"

#include <memory>


namespace Tmpl8
{
	enum class MoveDirection
	{
		VERTICAL,
		HORIZONTAL,
		NONE,
	};


	class MovingObstacle : public Obstacle
	{
	public:
		MovingObstacle(std::pair<MoveDirection, int>direction_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite, Surface& obstacle_layer, Surface& map_layer);
		
		void GetReady();
		void Update(float deltaTime);
		//void Draw(Surface* screen) override;
		void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity = 1.0f) override;

	private:
		CollidableType GetCollidableType(int tile_id);
		int GetDrawOrder(int tile_id);
		void UpdatePosition();
		void CutAndPaste() { };

		void RestoreMapLayer();
		void CopyMapLayer();

		int m_offset_index{ 0 };
		vec2 m_offset{ 0.0f, 0.0f };
		vec2 m_prev_offset{ 0.0f, 0.0f };
		int prev_left{ 0 }, prev_top{ 0 }, prev_right{ 0 }, prev_bottom{ 0 };

		int m_half_size{ 32 };
		float m_speed{ 0.2f };
		float m_elapsed_time{ 0.0f };
		int m_magnitude_coefficient{ 64 };
		int m_sign_of_direction{ 1 }; // start positive.

		Surface& m_obstacle_layer;
		Surface& m_map_layer;

		Surface m_clipboard;
		bool m_is_clipboard_ready{ false };
	};

};

