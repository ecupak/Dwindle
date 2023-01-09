#include "moving_obstacle.h"

#include "blueprint_code.h"


namespace Tmpl8
{
	MovingObstacle::MovingObstacle(std::pair<MoveDirection, int>direction_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite, Surface& obstacle_layer, Surface& map_layer) :
		Obstacle{ CollidableInfo{GetCollidableType(tile_id), CollisionLayer::BOTH, CollisionMask::NONE, GetDrawOrder(tile_id)}, x, y, TILE_SIZE, autotile_id, tile_id, sprite },
		// Get movement plan.
		m_sign_of_travel_direction{ direction_info.first == MoveDirection::LEFT ? -1 : 1 },
		m_travel_distance{ static_cast<float>((direction_info.second - 1) * TILE_SIZE) },
		m_min_offset{ direction_info.first == MoveDirection::LEFT ? -(m_travel_distance) : 0.0f },
		m_max_offset{ direction_info.first == MoveDirection::LEFT ? 0.0f : m_travel_distance },
		// Precalculated for collision box math.
		m_half_size{ TILE_SIZE / 2 },
		// For drawing.
		m_obstacle_layer{ obstacle_layer },
		m_map_layer{ map_layer },
		m_clipboard{ TILE_SIZE, TILE_SIZE }
	{	}


	CollidableType MovingObstacle::GetCollidableType(int tile_id)
	{
		return (tile_id == VISIBLE_MOVING_OBSTACLE_TILE
			? CollidableType::OBSTACLE_MOVING_VISIBLE
			: CollidableType::OBSTACLE_MOVING_HIDDEN
		);
	}


	int MovingObstacle::GetDrawOrder(int tile_id)
	{
		return (tile_id == VISIBLE_MOVING_OBSTACLE_TILE ? 7 : 0);
	}


	void MovingObstacle::Update(float deltaTime)
	{
		// Store last position for delta calculation.
		m_prev_offset.x = m_offset.x;

		m_offset.x += m_speed * deltaTime * m_sign_of_travel_direction;
		m_offset.x = Clamp(m_offset.x, m_min_offset, m_max_offset);

		if (m_offset.x == m_min_offset || m_offset.x == m_max_offset)
		{
			m_elapsed_time += deltaTime;

			if (m_elapsed_time >= 4.0f)
			{
				m_sign_of_travel_direction *= -1;
				m_elapsed_time = 0.0f;
			}
		}

		m_delta_position.x = m_offset.x - m_prev_offset.x;

		UpdatePosition();
	}


	void MovingObstacle::UpdatePosition()
	{
		m_prev_left = left;
		m_prev_right = right;
		m_prev_top = top;
		m_prev_bottom = bottom;

		m_center += m_delta_position;

		left = static_cast<int>(floor(m_center.x) - m_half_size);
		right = left + (m_half_size * 2);
		top = static_cast<int>(floor(m_center.y) - m_half_size);
		bottom = top + (m_half_size * 2);
	}


	void MovingObstacle::Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity)
	{
		m_sprite.SetFrame(m_frame_id);

		if (m_collidable_type == CollidableType::OBSTACLE_MOVING_VISIBLE)
		{
			m_sprite.Draw(visible_layer, left - c_left, top - c_top, true, opacity);
		}
		else
		{
			DrawOntoObstacleLayer();
			DrawOntoMapLayer();
			StoreLastDrawnCoordinates();
		}
	}


	void MovingObstacle::DrawOntoObstacleLayer()
	{
		// Restore and draw onto obstacle layer.
		m_obstacle_layer.Bar(m_last_drawn_left, m_last_drawn_top, m_last_drawn_right, m_last_drawn_bottom, 0x00000000);
		m_sprite.Draw(&m_obstacle_layer, left, top);
	}
	

	void MovingObstacle::DrawOntoMapLayer()
	{
		// Restore, copy draw area, and draw onto map layer.
		RestoreMapLayer();
		CopyMapLayer();
		m_sprite.Draw(&m_map_layer, left, top);
	}
	

	void MovingObstacle::StoreLastDrawnCoordinates()
	{
		m_last_drawn_left = left;
		m_last_drawn_right = right;
		m_last_drawn_top = top;
		m_last_drawn_bottom = bottom;
	}


	void MovingObstacle::RestoreMapLayer()
	{				
		if (m_is_clipboard_ready)
		{
			m_clipboard.CopyTo(&m_map_layer, m_last_drawn_left, m_last_drawn_top);
		}
	}

	
	void MovingObstacle::CopyMapLayer()
	{
		Pixel* clip_pix = m_clipboard.GetBuffer();
		Pixel* map_pix = m_map_layer.GetBuffer() + left + (top * m_map_layer.GetPitch());

		for (int y{ 0 }; y < m_clipboard.GetHeight(); ++y)
		{
			for (int x{ 0 }; x < m_clipboard.GetWidth(); ++x)
			{
				clip_pix[x] = map_pix[x];
			}
			clip_pix += m_clipboard.GetPitch();
			map_pix += m_map_layer.GetPitch();
		}

		m_is_clipboard_ready = true;
	}	
};