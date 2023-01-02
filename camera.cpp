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

		
	Socket<CameraMessage>* Camera::GetCameraSocket()
	{
		return &m_camera_hub;
	}


	void Camera::SetRevealedLayer(Surface& revealed_layer)
	{
		m_revealed_layer = &revealed_layer;
	}


	void Camera::SetLevelBounds(vec2& level_bounds)
	{
		m_level_size = level_bounds;
	}


	void Camera::ResolveCollision(Collidable*& collision)
	{
		m_collisions.push_back(collision);
	}
	

	void Camera::FadeToBlack()
	{
		m_is_fading_out = true;
	}


	void Camera::FadeIntoView()
	{
		m_opacity = 1.0f;
	}


	void Camera::Draw(Surface* visible_layer)
	{
		// Set bounds constrained by screen boundary.
		int inbound_left = Max(left, 0);
		int inbound_right = Min(right, static_cast<int>(m_level_size.x) - 1);
		int inbound_top = Max(top, 0);
		int inbound_bottom = Min(bottom, static_cast<int>(m_level_size.y) - 1);

		//Pixel* d_pix{ screen->GetBuffer() };
		Pixel* destination_pix{ visible_layer->GetBuffer() + (inbound_left - left) + ((inbound_top - top) * visible_layer->GetPitch())};
		Pixel* source_pix{ m_revealed_layer->GetBuffer() + inbound_left + (inbound_top * m_revealed_layer->GetPitch()) };
				
		for (int y{ inbound_top }; y < inbound_bottom; ++y)
		{
			for (int x{ 0 }; x < (inbound_right - inbound_left); ++x)
			{
				if (source_pix[x] > 0)
				{
					destination_pix[x] = MixAlpha(source_pix[x], m_opacity, 0xFF000000, false);
				}
			}
			destination_pix += visible_layer->GetPitch();
			source_pix += m_revealed_layer->GetPitch();
		}

		// Draw any part of a collidable that overlaps with view.
		for (Collidable*& collision : m_collisions)
		{
			collision->Draw(visible_layer, left, top, inbound_left, inbound_top, inbound_right, inbound_bottom);
		}
		m_collisions.clear();

		// Draw player (draw last so always on top).
		m_subject.Draw(visible_layer, left, top, inbound_left, inbound_top, inbound_right, inbound_bottom);


	}


	void Camera::Update(float deltaTime)
	{
		// Fade revealed layer on level reset/end.
		if (m_is_fading_out)
		{
			m_opacity -= opacity_delta * deltaTime;
			opacity_delta += opacity_delta_delta * deltaTime;

			m_opacity = Max(0.0f, m_opacity);
			m_is_fading_out = (m_opacity > 0.0f);
		}

		// Find new focus.
		if (m_camera_hub.HasNewMessage())
		{
			std::vector<CameraMessage>& messages = m_camera_hub.ReadMessages();

			m_focus.y = messages.back().m_new_center.y;

			m_subject_prev_location = m_subject_location;
			m_subject_location = messages.back().m_new_location;
			
			m_camera_hub.ClearMessages();


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
