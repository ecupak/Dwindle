#pragma once

//#include "player.h"
#include "collidable.h"
#include "template.h"
#include "camera_socket.h"

namespace Tmpl8
{
	class Camera : public Collidable
	{
	public:
		Camera(Collidable& subject);
		void Update(float deltaTime);
		void Draw(Surface* screen);
		void ResolveCollision(Collidable*& collision) override;
		Socket<CameraMessage>& GetPlayerCameraSocket();
		void SetPosition(vec2 position);
		void ProcessCollisions() {};
		void SetLevelBounds(vec2& level_bounds);

	private:
		void UpdateBounds();

		float speed{ 150.0f };

		std::vector<Collidable*> m_collisions;

		vec2 m_level_size{ 0.0f, 0.0f };

		Socket<CameraMessage> m_camera_socket;
		Collidable& m_subject;
		bool m_has_moved{ false };
		vec2 m_offset{ 0.0f, 0.0f };
		int offset_x{ 540 }; // 1/2 screen width;
		int offset_y{ 360 }; // 1/2 screen height;
		int m_dead_zone{ 4 };
		float m_deltaTime{ 0.0f };
		vec2 velocity{ 0.0f, 0.0f };
		vec2 m_focus{ 0.0f, 0.0f };

		bool m_is_slow_following{ false };
		
		Location m_subject_location{ Location::GROUND };
		Location m_subject_prev_location{ Location::OTHER };



		template <typename T> int GetSign(T val);
	};
};

