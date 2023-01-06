#include "light_pickup.h"

namespace Tmpl8
{
	LightPickup::LightPickup(int x, int y, int TILE_SIZE, CollidableType object_type, Surface& image, Socket<GlowMessage>* glow_socket) :
		Collidable{ object_type, 3, GetCenter(x, y, TILE_SIZE) },
		m_image{ image },
		m_glow_socket{ glow_socket }
	{
		SetCollidablesWantedBitflag(m_collidables_of_interest);

		m_half_height = static_cast<int>(m_image.GetHeight() * 0.5f);

		left = static_cast<int>(floor(center.x - (m_image.GetWidth() / 2.0f)));
		right = left + m_image.GetWidth();
		top = static_cast<int>(floor(center.y - m_half_height));
		bottom = top + m_image.GetHeight();

		m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, center, 1.0f, CollidableType::PICKUP_GLOW, m_id });
	}


	vec2 LightPickup::GetCenter(int x, int y, int TILE_SIZE)
	{
		return vec2{
			(x * TILE_SIZE * 1.0f),
			(y * TILE_SIZE * 1.0f)
		};
	}


	void LightPickup::Update(float deltaTime, float opacity)
	{		
		m_elapsed_time += deltaTime * 0.4f * m_sign_of_direction;

		if (m_elapsed_time < 0.0f || m_elapsed_time > 1.0f)
		{
			m_elapsed_time = Clamp(m_elapsed_time, 0.0f, 1.0f);
			m_sign_of_direction *= -1;
		}

		// Ease in/out formula (Bezier curve). Credit to Creak at https://stackoverflow.com/questions/13462001/ease-in-and-ease-out-animation-formula
		m_offset_y = m_magnitude_coefficient * ((m_elapsed_time * m_elapsed_time) * (3 - (2 * m_elapsed_time)) - 0.5f);

		UpdateYPosition();
		m_opacity = opacity;
	}

	
	void LightPickup::UpdateYPosition()
	{
		// Only y position changes.
		top = static_cast<int>(floor(center.y - m_half_height - m_offset_y));
		bottom = top + (m_half_height * 2);
	}


	void LightPickup::RegisterCollision(Collidable*& collision)
	{
		if (collision->m_object_type == CollidableType::PLAYER_POINT)
		{
			m_has_been_picked_up = true;
		}
	}


	void LightPickup::ResolveCollisions()
	{
		// If touched by player, mark for removal and tell accompanying glow orb to start fading away.
		if (m_has_been_picked_up && m_is_active)
		{
			m_is_active = false;
			m_glow_socket->SendMessage(GlowMessage{ GlowAction::REMOVE_ORB, m_id });
		}
	}


	void LightPickup::Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		// All passed in_x values are within screen bounds. Find bounds of object to draw within these bounds.
		int d_top = Max(top, in_top);
		int d_bottom = Min(bottom, in_bottom);
		int d_left = Max(left, in_left);
		int d_right = Min(right, in_right);

		/*
			Iterate over bounding box and store opacity value in pixel (in blue channel).
			Only store opacity value if the current value of pixel is less.
		*/

		Pixel* d_pix = viewable_layer->GetBuffer() + (d_left - c_left) + ((d_top - c_top) * viewable_layer->GetPitch());		
		
		// When drawing from isolated image to bigger surface, use this setup to get start position of source Pixel.
		Pixel* s_pix = m_image.GetBuffer() + (d_left - left) + ((d_top - top) * m_image.GetPitch());

		for (int y{ d_top }; y < d_bottom; y++)
		{
			for (int x{ 0 }; x < (d_right - d_left); x++)
			{
				if (s_pix[x] > 0)
				{
					if (m_opacity < 1.0f)
					{
						d_pix[x] = MixAlpha(s_pix[x], m_opacity, d_pix[x], false);
					}
					else
					{
						d_pix[x] = s_pix[x];
					}
				}
			}
			d_pix += viewable_layer->GetPitch();
			s_pix += m_image.GetPitch();
		}
	}

};