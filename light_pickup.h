#pragma once

#include "collidable.h"
#include "glow_socket.h"

namespace Tmpl8
{
	class LightPickup : public Collidable
	{
	public:
		LightPickup(int x, int y, int TILE_SIZE, CollidableType object_type, Surface& image, Socket<GlowMessage>* glow_socket);

		void Update(float deltaTime, float opacity);
		void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity = 1.0f) override;
		void RegisterCollision(Collidable*& collision) override;
		void ResolveCollisions() override;

	private:
		vec2 GetCenter(int x, int y, int TILE_SIZE);
		void UpdateYPosition();

		Surface& m_image;
		Socket<GlowMessage>* m_glow_socket;

		float m_speed{ 0.3f };
		float m_elapsed_time{ 0.5f }; // start in middle of curve (normal center).
		float m_offset_y{ 0 };
		float m_magnitude_coefficient{ 20.0f };
		int m_sign_of_travel_direction{ 1 }; // start positive.
		int m_half_height{ 0 };
		bool m_has_been_picked_up{ false };
		float m_opacity{ 1.0f };
	};
};

