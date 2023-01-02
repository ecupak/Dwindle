#pragma once

#include "surface.h"
#include "template.h"


namespace Tmpl8
{
	enum class CollidableType
	{
		PLAYER,
		SMOOTH,		
		UNREACHABLE,
		FINISH_LINE,
		PICKUP,
		QUICK_GLOW,
		FULL_GLOW,
		TEMP_GLOW,
		SAFE_GLOW,
		PERM_GLOW,
		PICKUP_GLOW,
		UNKNOWN,
		NONE,
	};

	class Collidable
	{
	public:
		Collidable() { };
		Collidable(int x, int y, int tile_size, CollidableType object_type);
		virtual void Draw(Surface* screen) {};
		virtual void Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom) {};
		virtual void GetCollisions() {};
		virtual void ResolveCollision(Collidable*& collision) {};

		// ATTRIBUTES
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
		vec2 center{ 0.0f, 0.0f };
		int m_id{ 0 };
		CollidableType m_object_type{ CollidableType::UNKNOWN };

	private:
		static int id_generator;
	};

};

