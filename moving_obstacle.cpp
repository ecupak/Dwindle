#include "moving_obstacle.h"


namespace Tmpl8
{
	MovingObstacle::MovingObstacle(std::pair<MoveDirection, int>direction_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite) :
		Obstacle{ CollidableInfo{CollidableType::OBSTACLE_MOVING, CollisionLayer::BOTH, CollisionMask::NONE, 7}, x, y, TILE_SIZE, autotile_id, tile_id, sprite },
		m_offset_index{ direction_info.first == MoveDirection::HORIZONTAL ? 0 : 1 },
		m_magnitude_coefficient{ (direction_info.second - 1) * TILE_SIZE },
		m_half_size{ TILE_SIZE / 2 }
	{	}


	void MovingObstacle::Update(float deltaTime)
	{
		m_elapsed_time += deltaTime * m_speed * m_sign_of_direction;

		if (m_elapsed_time < 0.0f || m_elapsed_time > 1.0f)
		{
			m_elapsed_time = Clamp(m_elapsed_time, 0.0f, 1.0f);
			m_sign_of_direction *= -1;
		}

		// Ease in/out formula (Bezier curve). Credit to Creak at https://stackoverflow.com/questions/13462001/ease-in-and-ease-out-animation-formula
		m_prev_offset[m_offset_index] = m_offset[m_offset_index];
		m_offset[m_offset_index] = m_magnitude_coefficient * (m_elapsed_time * m_elapsed_time) * (3 - (2 * m_elapsed_time));

		m_delta_position[m_offset_index] = m_offset[m_offset_index] - m_prev_offset[m_offset_index];

		UpdatePosition();
	}

	void MovingObstacle::Draw(Surface* layer)
	{
		m_sprite.SetFrame(m_frame_id);
		m_sprite.Draw(layer, left, top);
	}


	void MovingObstacle::UpdatePosition()
	{
		m_center += m_delta_position;

		left = static_cast<int>(m_center.x - m_half_size);
		right = left + (m_half_size * 2);
		top = static_cast<int>(m_center.y - m_half_size);
		bottom = top + (m_half_size * 2);
	}
};