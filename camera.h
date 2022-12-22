#pragma once

#include "collidable.h"

namespace Tmpl8
{
	class Camera : public Collidable
	{
	public:
		Camera(Collidable& subject);
		void Update(float deltaTime);
		void Draw(Surface* screen);
		
		void ResolveCollision(Collidable*& collision) override;
		void SetPosition(vec2 position);
		bool HasMoved();

	private:
		void BringSubjectIntoFocus(float deltaTime);
		void UpdateBounds();

		Collidable& m_subject;
		bool is_refocusing{ false };
		bool m_has_moved{ false };
		int offset_x{ 540 };
		int offset_y{ 360 };
		int m_dead_zone{ 4 };
		float m_deltaTime{ 0.0f };
		vec2 velocity{ 0.0f, 0.0f };
		vec2 m_focus{ 0.0f, 0.0f };
	};
};

