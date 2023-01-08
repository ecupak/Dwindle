#include "camera.h"

#include <cmath>
#include <algorithm>


namespace Tmpl8
{
	Camera::Camera(Collidable& subject) :
		Collidable{ CollidableInfo{CollidableType::CAMERA, CollisionLayer::CAMERA, CollisionMask::CAMERA, 0 } },
		m_subject{ subject }
	{
		m_offset.x = floor(ScreenWidth * 0.5f);
		m_offset.y = floor(ScreenHeight * 0.5f);
	}


	void Camera::SetCenter(vec2 center)
	{
		m_center = center;
		m_focus = m_center;
		UpdateBounds();
		m_has_moved = true;
	}

		
	Socket<CameraMessage>* Camera::GetCameraSocket()
	{
		return &m_camera_hub;
	}


	void Camera::RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket)
	{
		m_collision_socket = collision_socket;
	}


	void Camera::RegisterWithCollisionManager()
	{
		m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_UNIQUE_LIST, this });
	}


	void Camera::SetRevealedLayer(Surface& revealed_layer)
	{
		m_revealed_layer = &revealed_layer;
	}


	void Camera::SetLevelBounds(vec2& level_bounds)
	{
		m_level_size = level_bounds;
	}


	void Camera::RegisterCollision(Collidable*& collision)
	{
		m_collisions.push_back(collision);
	}
	

	void Camera::ResolveCollisions()
	{
		// Order by drawing layer. Low numbers are drawn first.
		std::sort(m_collisions.begin(), m_collisions.end(),
			[=](Collidable* a, Collidable* b) { return a->m_draw_order < b->m_draw_order; });
	}


	void Camera::FadeToBlack()
	{
		m_is_fading_out = true;
	}


	void Camera::RestoreView()
	{
		m_is_fading_out = false;
		m_opacity = 1.0f;
		opacity_delta = 0.0f;
	}


	void Camera::Update(float deltaTime)
	{
		// Fade revealed layer on level reset/end.
		FadeOpacity(deltaTime);		

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
		m_focus.x = m_subject.m_center.x;

		// Only follow y position if we just went from ground to wall, ground to higher ground, from wall to anything, or have fallen below previous y position.
		if (m_is_slow_following || m_subject.m_center.y > m_focus.y || m_subject_location == Location::WALL)
		{
			m_focus.y = m_subject.m_center.y;
		}

		// Move to new focus.
		m_has_moved = false;
				
		float dist_x{ m_focus.x - m_center.x };
		float dist_y{ m_focus.y - m_center.y };
		float distance_to_move = speed * deltaTime;

		if (dist_x != 0.0f)
		{
			m_center.x = m_focus.x;
			m_has_moved = true;
		}
		

		if (dist_y != 0.0f)
		{
			if (m_is_slow_following && std::abs(dist_y) > distance_to_move)
			{
				m_center.y += GetSign(dist_y) * distance_to_move;
			}
			else
			{
				m_center.y = m_focus.y;
				if (m_is_slow_following)
					m_is_slow_following = false;
			}
			m_has_moved = true;
		}

		UpdateBounds();
	}

	
	void Camera::FadeOpacity(float deltaTime)
	{
		if (m_is_fading_out)
		{
			if (m_opacity == 0.0f) return;

			// Fade out.
			m_opacity -= opacity_delta * deltaTime;
			opacity_delta += opacity_delta_delta * deltaTime;
			m_opacity = Max(0.0f, m_opacity);
		}
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
		left = m_center.x - m_offset.x;
		right = left + (m_offset.x * 2);
		top = m_center.y - m_offset.y;
		bottom = top + (m_offset.y * 2);
	}


	void Camera::Draw(Surface* visible_layer)
	{
		// Set bounds constrained by screen edges.
		int inbound_left = Max(left, 0);
		int inbound_right = Min(right, static_cast<int>(m_level_size.x) - 1);
		int inbound_top = Max(top, 0);
		int inbound_bottom = Min(bottom, static_cast<int>(m_level_size.y) - 1);		

		// First, draw everything that is static (visible ground, visible text).
		Draw(visible_layer, left, top, inbound_left, inbound_top, inbound_right, inbound_bottom);

		// Second, draw dynamic objects that move (glow orbs, player, moving obstacles). 
		for (Collidable*& collision : m_collisions)
		{
			collision->Draw(visible_layer, left, top, inbound_left, inbound_top, inbound_right, inbound_bottom);
		}
		m_collisions.clear();
	}


	void Camera::Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		Pixel* destination_pix{ visible_layer->GetBuffer() + (in_left - left) + ((in_top - top) * visible_layer->GetPitch()) };
		Pixel* source_pix{ m_revealed_layer->GetBuffer() + in_left + (in_top * m_revealed_layer->GetPitch()) };

		for (int y{ in_top }; y < in_bottom; ++y)
		{
			for (int x{ 0 }; x < (in_right - in_left); ++x)
			{
				if (source_pix[x] > 0)
				{
					destination_pix[x] = MixAlpha(source_pix[x], m_opacity, 0xFF000000, false);
				}
			}
			destination_pix += visible_layer->GetPitch();
			source_pix += m_revealed_layer->GetPitch();
		}
	}
};
