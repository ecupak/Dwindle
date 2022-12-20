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
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
		vec2 center{ 0.0f, 0.0f };
		int m_id{ 0 };
		CollidableType m_object_type{ CollidableType::UNKNOWN };

	private:
		static int id_generator;
	};

};

