#pragma once

#include <vector>
#include <cstdio> //printf
#include "surface.h"
#include "template.h"


namespace Tmpl8
{
	enum class CollidableType
	{
		PLAYER,
		SMOOTH,
		SMOOTH_CORNER,
		ROUGH,
		ROUGH_CORNER,
		UNREACHABLE,
		UNKNOWN,
	};

	class Collidable
	{
	public:
		Collidable();
		virtual void Draw(Surface* screen) {}; 
		virtual void GetCollisions() {};
		virtual void ResolveCollision(Collidable*& collision) {};
		virtual void ResolveCollision(std::vector<Collidable*> collisions) {};

		// ATTRIBUTES
		float left{ 0.0f }, right{ 0.0f }, top{ 0.0f }, bottom{ 0.0f };
		vec2 center{ 0.0f, 0.0f };
		int m_id{ 0 };
		CollidableType m_object_type{ CollidableType::UNKNOWN };

	private:
		static int id_generator;

	};

};

