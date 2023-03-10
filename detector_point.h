#pragma once

#include <vector>

#include "collidable.h"
#include "standard_form.h"
#include "intersection.h"
#include "glow_orb.h"


namespace Tmpl8
{
	enum class State // Player state based on life/hp.
	{
		ALIVE,
		DEAD,
	};

	enum class InteractionType
	{
		SURFACE,
		SAFETY,
		PICKUP,
		LEVEL_END,
		NONE,
	};

	class DetectorPoint : public Collidable
	{
	public:
		DetectorPoint(int assigned_point);
		void SetPosition(vec2& m_center, int radius);
		void UpdatePosition(vec2& player_velocity, vec2& distance);
		void UpdateCollisionBox();
		
		void RegisterCollision(Collidable*& collision) override;
		void ResolveCollisions() {};

		bool CheckForCollisions();
		vec2& GetDeltaPosition();
		int GetNewMode();		
		int GetCollisionPointBinary();
		void ApplyDeltaPosition(vec2& delta_position);
		vec2& GetNewVelocity();
		void UpdatePreviousPosition();
		void ClearCollisions();
		void ResetValues();
		
		void UpdateState(State new_state);

		bool m_is_collision_detected{ false };

		vec2 position;
		vec2 prev_position;

		//vec2 i_position{ 0.0f, 0.0f };
		//vec2 i_prev_position{ 0.0f, 0.0f };
				
		int post_id;

		std::vector<Collidable*> m_obstacles;
		std::vector<Collidable*> m_glow_orbs;

		vec2 velocity{ 0.0f, 0.0f };
		int m_new_mode{ 0 };
		bool m_is_safe_glow_needed{ false };
		bool m_is_on_dangerous_obstacle{ false };
		bool m_is_at_finish_line{ false };
		bool m_is_on_pickup{ false };
		bool m_is_ricochet_collisions{ false };
		
		bool m_is_tethered{ false };
		Collidable* m_tethered_object;


		InteractionType m_interaction_type{ InteractionType::NONE };

	private:
		void CheckForIntersection(std::vector<Intersection>& intersects, StandardForm& line1, Collidable*& collision_object, EdgeCrossed collision_edge_crossed);
		void ConvertLineToStandardForm(Collidable*& collision_object, StandardForm& std_form, EdgeCrossed& collision_edge_crossed);
		bool GetIsCollisionPossible(StandardForm& line1, StandardForm& line2);
		vec2 GetIntersection(StandardForm& line1, StandardForm& line2);
		bool GetIsIntersectionInBounds(vec2& intersection, Collidable*& collision_object);

		void DetermineNextMode();		
		void DetermineIsSafeGlowNeeded();
		void DetermineIsTethered(Intersection& intersection_info);
		void DetermineIsOnDangerousObstacle(Intersection& intersection_info);

		bool GetIsRicochetCollision(EdgeCrossed& collision_edge_crossed, int neighbors);

		void ResolveSmoothCollision(Intersection& intersection_info);
		void SetRicochetSpeed(Intersection& intersection_info);

		vec2 GetCollisionBuffer(EdgeCrossed collision_edge_crossed);
		int GetPenetrationDepth(Collidable*& collision_object, EdgeCrossed collision_edge_crossed);

		int collision_count{ 0 };
		State m_state{ State::ALIVE };
	};
};

