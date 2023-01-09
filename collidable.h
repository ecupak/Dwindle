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
		OBSTACLE_VISIBLE,
		OBSTACLE_HIDDEN,
		OBSTACLE_DANGEROUS,
		OBSTACLE_MOVING_VISIBLE,
		OBSTACLE_MOVING_HIDDEN,
		OBSTACLE_UNREACHABLE,
		FINISH_LINE,
		PICKUP,
		GLOW_ORB_FULL,
		GLOW_ORB_TEMP,
		GLOW_ORB_SAFE,
		GLOW_ORB_PICKUP,
		UNKNOWN,
		NONE,
	};

	enum class CollisionType
	{		
		PLAYER,
		CAMERA,
		BOTH,
		NONE,
	};


	struct CollidableInfo
	{
		CollidableInfo(CollidableType collidable_type, CollisionType collision_layer, CollisionType collision_mask, int draw_order, vec2 m_center = vec2{ 0.0f, 0.0f }) :
			m_collidable_type{ collidable_type },
			m_collision_layer{ collision_layer },
			m_center{ m_center },
			m_draw_order{ draw_order }
		{	
			m_collision_mask.push_back(collision_mask);
		};

		CollidableInfo(CollidableType collidable_type, CollisionType collision_layer, std::vector<CollisionType>& m_collision_mask, int draw_order, vec2 m_center = vec2{ 0.0f, 0.0f }) :
			m_collidable_type{ collidable_type },
			m_collision_layer{ collision_layer },
			m_collision_mask{ m_collision_mask },
			m_center{ m_center },
			m_draw_order{ draw_order }
		{	};

		CollidableType m_collidable_type{ CollidableType::NONE };
		CollisionType m_collision_layer{ CollisionType::NONE };
		std::vector<CollisionType> m_collision_mask;
		vec2 m_center{ 0.0f, 0.0f };
		int m_draw_order{ 0 };
	};


	class Collidable
	{
		using CollisionLayer = CollisionType;
		using CollisionMask = CollisionType;

	public:
		Collidable(CollidableInfo collidable_info);
		Collidable(CollidableInfo collidable_info, int x, int y, int tile_size);

		int GetCollisionId(std::vector<CollisionType>& collidables);
		int GetCollisionId(CollisionType& collidable);

		virtual void Draw(Surface* screen) {};
		virtual void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity = 1.0f) {};
		virtual void RegisterCollision(Collidable*& collision) {};
		virtual void ResolveCollisions() {};

		// ATTRIBUTES
		int m_id{ 0 };
		CollidableType m_collidable_type{ CollidableType::UNKNOWN };
		int m_collision_layer{ 0 };
		int m_collision_mask{ 0 };
		vec2 m_center{ 0.0f, 0.0f };
		vec2 m_delta_position{ 0.0f, 0.0f };
		int m_draw_order{ 0 };
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
		int m_neighbors{ 0 };
		bool m_is_active{ true };
		
	private:
		static int id_generator;
	};

};

