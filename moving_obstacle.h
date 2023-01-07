#pragma once

#include "obstacle.h"


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
		MovingObstacle(std::pair<MoveDirection, int>direction_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite);

		void Update(float deltaTime);
		void Draw(Surface* screen) override;

	private:
		void UpdatePosition();

		int m_offset_index{ 0 };
		vec2 m_offset{ 0.0f, 0.0f };
		vec2 m_prev_offset{ 0.0f, 0.0f };

		int m_half_size{ 32 };
		float m_speed{ 0.2f };
		float m_elapsed_time{ 0.0f };
		int m_magnitude_coefficient{ 64 };
		int m_sign_of_direction{ 1 }; // start positive.
	};

};

