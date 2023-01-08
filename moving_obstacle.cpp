#include "moving_obstacle.h"

#include "blueprint_code.h"


namespace Tmpl8
{
	MovingObstacle::MovingObstacle(std::pair<MoveDirection, int>direction_info, int x, int y, int TILE_SIZE, int autotile_id, int tile_id, Sprite& sprite, Surface& obstacle_layer, Surface& map_layer) :
		Obstacle{ CollidableInfo{GetCollidableType(tile_id), CollisionLayer::BOTH, CollisionMask::NONE, GetDrawOrder(tile_id)}, x, y, TILE_SIZE, autotile_id, tile_id, sprite },
		m_obstacle_layer{ obstacle_layer },
		m_map_layer{ map_layer },
		m_offset_index{ direction_info.first == MoveDirection::HORIZONTAL ? 0 : 1 },
		m_magnitude_coefficient{ (direction_info.second - 1) * TILE_SIZE },
		m_half_size{ TILE_SIZE / 2 },
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


	void MovingObstacle::UpdatePosition()
	{
		prev_left = left;
		prev_right = right;
		prev_top = top;
		prev_bottom = bottom;

		m_center += m_delta_position;

		left = static_cast<int>(m_center.x - m_half_size);
		right = left + (m_half_size * 2);
		top = static_cast<int>(m_center.y - m_half_size);
		bottom = top + (m_half_size * 2);
	}


	//// Draw called by level to draw static 
	//void MovingObstacle::Draw(Surface* layer)
	//{
	//	m_sprite.SetFrame(m_frame_id);
	//	m_sprite.Draw(layer, left, top);
	//}


	void MovingObstacle::Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		m_sprite.SetFrame(m_frame_id);

		if (m_collidable_type == CollidableType::OBSTACLE_MOVING_VISIBLE)
		{
			m_sprite.Draw(visible_layer, left - c_left, top - c_top);
		}
		else
		{
			m_obstacle_layer.Bar(prev_left, prev_top, prev_right, prev_bottom, 0x00000000);
			m_sprite.Draw(&m_obstacle_layer, left, top);

			RestoreMapLayer();
			CopyMapLayer();
			m_sprite.Draw(&m_map_layer, left, top);
		}
	}


	void MovingObstacle::RestoreMapLayer()
	{
		// Copy clipboard contents onto map layer.
		
		if (m_is_clipboard_ready)
		{
			m_clipboard.CopyTo(&m_map_layer, prev_left, prev_top);
		}

		//Pixel* map_pix = m_map_layer.GetBuffer() + left + (top * m_map_layer.GetPitch());
		//Pixel* clip_pix = m_clipboard.GetBuffer();

		//for (int y{ 0 }; y < m_clipboard.GetHeight(); ++y)
		//{
		//	for (int x{ 0 }; x < m_clipboard.GetWidth(); ++x)
		//	{
		//		map_pix[x] = clip_pix[x];
		//	}
		//	map_pix += m_map_layer.GetPitch();
		//	clip_pix += m_clipboard.GetPitch();
		//}
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