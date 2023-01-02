#pragma once

#include "collidable.h"
#include "glow_orb.h"

namespace Tmpl8
{
	class LightPickup : public Collidable
	{
	public:
		LightPickup(int x, int y, int TILE_SIZE, CollidableType object_type, Surface& image, GlowOrb& glow_orb);
		void Update(float deltaTime);
		void Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom) override;		

	private:
		Surface& m_image;
		GlowOrb& m_glow_orb;
		float m_elapsed_time{ 0.0f };
		int m_sign_of_direction{ 1 }; // start positive.

	};
};

