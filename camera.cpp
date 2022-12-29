#include "camera.h"
#include <cmath>

namespace Tmpl8
{
	Camera::Camera(Collidable& subject) :
		m_subject{ subject }
	{
		m_offset.x = floor(ScreenWidth * 0.5f);
		m_offset.y = floor(ScreenHeight * 0.5f);
	}


	void Camera::SetPosition(vec2 position)
	{
		center = position;
		m_focus = center;
		UpdateBounds();
		m_has_moved = true;
	}

		
	Socket<CameraMessage>& Camera::GetPlayerCameraSocket()
	{
		return m_camera_socket;
	}

	void Camera::SetLevelBounds(vec2& level_bounds)
	{
		m_level_size = level_bounds;
	}

	void Camera::ResolveCollision(Collidable*& collision)
	{
		m_collisions.push_back(collision);
	}
	

	void Camera::Draw(Surface* screen)
	{
		// Set bounds constrained by screen boundary.
		float inbound_left = Max(left, 0);
		float inbound_right = Min(right, static_cast<int>(m_level_size.x) - 1);
		float inbound_top = Max(top, 0);
		float inbound_bottom = Min(bottom, static_cast<int>(m_level_size.y) - 1);

		// Draw any part of a collidable that overlaps with view.
		for (Collidable*& collision : m_collisions)
		{
			collision->Draw(screen, left, top, inbound_left, inbound_top, inbound_right, inbound_bottom);
		}
		m_collisions.clear();

		// Draw player (draw last so always on top).
		m_subject.Draw(screen, left, top, inbound_left, inbound_top, inbound_right, inbound_bottom);

		//// Camera bounds.
		//screen->Box(left - left, top - top, right - left, bottom - top, 0xFFFF0000);
		//// Crosshairs.
		//screen->Line(left - left, center.y - top, right - left, center.y - top, 0x000000FF);
		//screen->Line(center.x - left, top - top, center.x - left, bottom - top, 0x000000FF);
		//// Focus box.
		//screen->Box(m_focus.x - 10 - left, m_focus.y - 10 - top, m_focus.x + 10 - left, m_focus.y + 10 - top, 0x00FF0000);
	}


	void Camera::Update(float deltaTime)
	{
		// Find new focus.
		if (m_camera_socket.HasNewMessage())
		{
			std::vector<CameraMessage>& messages = m_camera_socket.ReadMessages();

			m_focus.y = messages.back().m_new_center.y;

			m_subject_prev_location = m_subject_location;
			m_subject_location = messages.back().m_new_location;
			
			m_camera_socket.ClearMessages();


			// Follow y position exactly (when following) unless it is initial cling to wall from ground.
			// Because we don't follow every y move, we do not know we are in a fast-follow y state until
			// a trigger happens.
			if (m_subject_prev_location == Location::GROUND && (m_subject_location == Location::WALL || m_subject_location == Location::GROUND))
			{
				m_is_slow_following = true;
			}
		}
		
		// Always follow x position exactly.
		m_focus.x = m_subject.center.x;

		// Only follow y position if we just went from ground to wall, ground to higher ground, from wall to anything, or have fallen below previous y position.
		if (m_is_slow_following || m_subject.center.y > m_focus.y || m_subject_location == Location::WALL)
		{
			m_focus.y = m_subject.center.y;
		}

		// Move to new focus.
		m_has_moved = false;
				
		float dist_x{ m_focus.x - center.x };
		float dist_y{ m_focus.y - center.y };
		float distance_to_move = speed * deltaTime;

		if (dist_x != 0.0f)
		{
			center.x = m_focus.x;
			m_has_moved = true;
		}
		

		if (dist_y != 0.0f)
		{
			if (m_is_slow_following && std::abs(dist_y) > distance_to_move)
			{
				center.y += GetSign(dist_y) * distance_to_move;
			}
			else
			{
				center.y = m_focus.y;
				if (m_is_slow_following)
					m_is_slow_following = false;
			}
			m_has_moved = true;
		}

		UpdateBounds();
	}
	

	/*
		Credit to user79785: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
	*/
	template <typename T>
	int Camera::GetSign(T val) {
		return (T(0) < val) - (val < T(0));
	}


	void Camera::UpdateBounds()
	{
		left = center.x - m_offset.x;
		right = center.x + m_offset.x;
		top = center.y - m_offset.y;
		bottom = center.y + m_offset.y;
	}
};
