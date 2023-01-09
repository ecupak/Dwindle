#pragma once

#include "collidable.h"
#include "template.h"

namespace Tmpl8
{
	enum class EdgeCrossed // Side of obstacle player hit.
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		NONE,
	};

	class Intersection
	{
	public:
		// Structors.
		Intersection::Intersection() :
			m_collision_object{ nullptr },
			m_intersection{ 0.0f, 0.0f },
			m_collision_edge_crossed{ EdgeCrossed::NONE },
			m_type_of_collision_object{ CollidableType::UNKNOWN },
			m_penetration{ 0 }
		{	}

		Intersection::Intersection(Collidable*& collision_object, vec2 intersection, EdgeCrossed collision_edge_crossed, int penetration) :
			m_collision_object{ collision_object },
			m_intersection{ intersection },
			m_collision_edge_crossed{ collision_edge_crossed },
			m_type_of_collision_object{ collision_object->m_collidable_type },
			m_penetration{ penetration }
		{	}

		// Methods.
		int Intersection::GetHorizontalRicochetDirection()
		{
			float dist_to_left{ fabsf(m_intersection.x - m_collision_object->left) };
			float dist_to_right{ fabsf(m_intersection.x - m_collision_object->right) };

			return (dist_to_left < dist_to_right ? -1 : 1);
		}

		// Attributes.
		Collidable* m_collision_object;
		vec2 m_intersection{ 0.0f, 0.0f };
		EdgeCrossed m_collision_edge_crossed;
		CollidableType m_type_of_collision_object;
		int m_penetration{ 0 };
	};
};