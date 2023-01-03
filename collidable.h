#pragma once

#include <vector>

#include "surface.h"
#include "template.h"

namespace Tmpl8
{
	enum class CollidableType
	{
		PLAYER,
		PLAYER_POINT,
		CAMERA,
		OBSTACLE,		
		UNREACHABLE,
		FINISH_LINE,
		PICKUP,
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
		Collidable(CollidableType object_type, int draw_order, vec2 center = vec2{ 0.f, 0.f });
		Collidable(int x, int y, int tile_size, CollidableType object_type, int draw_order, vec2 center = vec2{ 0.f, 0.f });

		void SetCollidablesWantedBitflag(std::vector<CollidableType>& collidables_of_interest);

		int GetCollisionId(std::vector<CollidableType>& collidables);
		int GetCollisionId(CollidableType& collidable);

		virtual void Draw(Surface* screen) {};
		virtual void Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom) {};
		virtual void RegisterCollision(Collidable*& collision) {};
		virtual void ResolveCollisions() {};

		// ATTRIBUTES
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
		vec2 center{ 0.0f, 0.0f };
		int m_id{ 0 };
		CollidableType m_object_type{ CollidableType::UNKNOWN };
		bool m_is_active{ true };

		int m_collision_id{ 0 };
		int m_collision_ids_wanted{ 0 };
		int m_draw_order{ 0 };

	private:
		static int id_generator;
	};

};

