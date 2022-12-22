#pragma once

#include <vector>

#include "collidable.h"
#include "standard_form.h"
#include "intersection.h"

namespace Tmpl8
{
	class DetectorPoint : public Collidable
	{
	public:
		DetectorPoint(int assigned_point);
		void SetPosition(vec2& center, int radius);
		void UpdatePosition(vec2& player_speed);
		void UpdateCollisionBox();
		virtual void ResolveCollision(Collidable*& collision);
		bool CheckForCollisions();
		vec2& GetDeltaPosition();
		int GetNewMode();		
		int GetCollisionPointBinary();
		void ApplyDeltaPosition(vec2& delta_position);
		vec2& GetNewSpeed();
		void UpdatePreviousPosition();

		vec2 position;
		vec2 prev_position;

		vec2 i_position{ 0.0f, 0.0f };
		vec2 i_prev_position{ 0.0f, 0.0f };

		vec2 delta_position{ 0.0f, 0.0f };
		int post;
		std::vector<Collidable*> collisions;
		vec2 speed{ 0.0f, 0.0f };
		int new_mode{ 0 };
		bool isRicochetCollision{ false };

	private:
		void CheckForIntersection(std::vector<Intersection>& intersects, StandardForm& line1, Collidable*& collision_object, EdgeCrossed collision_edge_crossed);
		void ConvertLineToStandardForm(Collidable*& collision_object, StandardForm& std_form, EdgeCrossed& collision_edge_crossed);
		bool GetIsCollisionPossible(StandardForm& line1, StandardForm& line2);
		vec2 GetIntersection(StandardForm& line1, StandardForm& line2);
		bool GetIsIntersectionInBounds(vec2& intersection, Collidable*& collision_object);

		constexpr int GetNextMode();
		bool GetIsRicochetCollision(EdgeCrossed& collision_edge_crossed);

		void ResolveSmoothCollision(Intersection& intersection_info);
		void ResolveSmoothCornerCollision(vec2& ricochet_direction);
		void ResolveRoughCollision();
		void ResolveRoughCornerCollision();
		void SetRicochetSpeed(Intersection& intersection_info);

		vec2 GetCollisionBuffer(EdgeCrossed collision_edge_crossed);
		int GetPenetrationDepth(Collidable*& collision_object, EdgeCrossed collision_edge_crossed);


		int collision_count{ 0 };
	};
};

