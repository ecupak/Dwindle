#include "light_pickup.h"

namespace Tmpl8
{
	LightPickup::LightPickup(int x, int y, int TILE_SIZE, CollidableType object_type, Surface& image, Socket<GlowMessage>* glow_socket) :
		m_image{ image },
		m_glow_socket{ glow_socket }
	{
		left = x * TILE_SIZE;
		right = left + TILE_SIZE - 1;
		top = y * TILE_SIZE;
		bottom = top - TILE_SIZE - 1;

		center = vec2{
			left + TILE_SIZE / 2.0f,
			top + TILE_SIZE / 2.0f
		};

		m_object_type = object_type;

		m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, center, 1.0f, CollidableType::PICKUP_GLOW, m_id });
	}


	void LightPickup::Update(float deltaTime)
	{		
		m_elapsed_time += deltaTime * m_sign_of_direction;

		if (m_elapsed_time < 0.0f || m_elapsed_time > 1.0f)
		{
			m_elapsed_time = Clamp(m_elapsed_time, 0.0f, 1.0f);
			m_sign_of_direction *= -1;
		}

		center.x = (m_elapsed_time * m_elapsed_time) * (3 - (2 * m_elapsed_time));
	}


	void LightPickup::ResolveCollision(Collidable*& collision)
	{
		// If touched by player, mark for removal and tell accompanying glow orb to start fading away.
		m_is_active = false;
		if (collision->m_object_type == CollidableType::PLAYER)
		{
			m_glow_socket->SendMessage(GlowMessage{ GlowAction::REMOVE_ORB, m_id });
		}
	}


	void LightPickup::Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		// >>>>> from here
		
		// viewable_layer bounds are already clamped to window bounds and are ints.
		// use unaltered results for h_pix math.
		// v_pix needs to subtract v_left and v_top.
		int d_top = Max(top, in_top);
		int d_bottom = Min(bottom, in_bottom);
		int d_left = Max(left, in_left);
		int d_right = Min(right, in_right);

		/*
			Iterate over bounding box and store opacity value in pixel (in blue channel).
			Only store opacity value if the current value of pixel is less.
		*/

		Pixel* d_pix = viewable_layer->GetBuffer() + (d_left - c_left) + ((d_top - c_top) * viewable_layer->GetPitch());		

		// <<<<< to here: make part of base class? not override. add another Draw() to derived class, that calls base class Draw first with parameters?
		
		Pixel* s_pix = m_image.GetBuffer() + (d_left - c_left) + ((d_top - c_top) * m_image.GetPitch());

		for (int y{ d_top }; y < d_bottom; y++)
		{
			for (int x{ 0 }; x <= (d_right - d_left); x++)
			{
				d_pix[x] = s_pix[x];
			}
		}
	}

};