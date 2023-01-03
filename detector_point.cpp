#include "detector_point.h"

#include <cstdio> //printf
#include <cmath> // floor

namespace Tmpl8
{
	// Constants.
	/* Post positions around edge of sprite. */
	constexpr int RIGHT{ 0 };
	constexpr int TOP_RIGHT{ 1 };
	constexpr int TOP{ 2 };
	constexpr int TOP_LEFT{ 3 };
	constexpr int LEFT{ 4 };
	constexpr int BOTTOM_LEFT{ 5 };
	constexpr int BOTTOM{ 6 };
	constexpr int BOTTOM_RIGHT{ 7 };
	constexpr int POINTS{ 8 };
	/* Bitwise values of mode after collision. */
	constexpr int NONE{ 0 };
	constexpr int GROUND{ 1 };
	constexpr int WALL{ 2 };
	constexpr int CEILING{ 4 };
	/* Ricochet x,y modulators. */
	constexpr float LOW_X_FACTOR{ 0.25f };
	constexpr float MED_X_FACTOR{ 0.5f };
	constexpr float HIGH_X_FACTOR{ 0.75f };
	constexpr float LOW_Y_FACTOR{ 2.0f };
	constexpr float MED_Y_FACTOR{ 3.0f };
	constexpr float HIGH_Y_FACTOR{ 4.0f };


	DetectorPoint::DetectorPoint(int assigned_post) :
		Collidable{ CollidableType::PLAYER_POINT, 0 },
		post_id{ assigned_post }
	{	
		SetCollidablesWantedBitflag(m_collidables_of_interest);
	}


	void DetectorPoint::SetPosition(vec2& center, int radius)
	{
		float deg45{ (2 * PI) / 8 };

		position.x = center.x + radius * cos(deg45 * post_id);
		position.y = center.y - radius * sin(deg45 * post_id);
		prev_position = position;

		UpdateCollisionBox();
	}


	void DetectorPoint::UpdatePosition(vec2& player_velocity, vec2& distance)
	{
		velocity = player_velocity;

		prev_position = position;
		position += distance;
				
		UpdateCollisionBox();
	}


	void DetectorPoint::UpdateCollisionBox()
	{
		/*
			+---+---------------+	Collision box is used in the
			|Pre|               |	sweep-and-prune test, as well
			+---+               |	as in the line-intercept test.
			|               +---+
			|               |Cur|	The previous and current positions
			+---------------+---+	set the bounds of the collision box.

			We are in pixel-space and will only use whole numbers.
		*/

		left = (int)floor(prev_position.x < position.x ? prev_position.x : position.x);
		right = (int)floor(prev_position.x < position.x ? position.x : prev_position.x);
		top = (int)floor(prev_position.y < position.y ? prev_position.y : position.y);
		bottom = (int)floor(prev_position.y < position.y ? position.y : prev_position.y);
	}


	void DetectorPoint::RegisterCollision(Collidable*& collision)
	{
		switch (collision->m_object_type)
		{
		case CollidableType::OBSTACLE:
			m_obstacles.push_back(collision);
			break;
		case CollidableType::SAFE_GLOW:
			m_glow_orbs.push_back(collision);
			break;
		case CollidableType::PERM_GLOW:
			m_obstacles.push_back(collision);
			m_glow_orbs.push_back(collision);
			break;
		case CollidableType::PICKUP:
			m_is_on_pickup = true;
			break;
		case CollidableType::FINISH_LINE:
			if (m_state == State::ALIVE)
			{
				m_is_at_finish_line = true;
			}
			else if (m_state == State::DEAD && m_is_at_finish_line == false)
			{
				m_obstacles.push_back(collision);
			}
			break;
		default:
			break;
		}
	}


	vec2& DetectorPoint::GetDeltaPosition()
	{
		return delta_position;
	}


	int DetectorPoint::GetNewMode()
	{
		return new_mode;
	}


	vec2& DetectorPoint::GetNewVelocity()
	{
		return velocity;
	}


	int DetectorPoint::GetCollisionPointBinary()
	{
		return (post_id == 0 ? 0 : 1 << (post_id - 1));
	}


	void DetectorPoint::ApplyDeltaPosition(vec2& delta_position)
	{
		position += delta_position;

		UpdateCollisionBox();
	}


	void DetectorPoint::ClearCollisions()
	{
		if (m_obstacles.size() > 0)
		{
			m_obstacles.clear();
		}

		if (m_glow_orbs.size() > 0)
		{
			m_glow_orbs.clear();
		}

		ResetValues();
	}


	void DetectorPoint::UpdatePreviousPosition()
	{
		/*
			Needed for when player enters non-AIR state and stops updating position.
			If not done, prev position may stay inside an object.	
		*/
		prev_position = position;
	}


	void DetectorPoint::UpdateState(State new_state)
	{
		m_state = new_state;		
	}


	void DetectorPoint::ResetValues()
	{
		delta_position.x = 0.0f;
		delta_position.y = 0.0f;
		m_is_collision_detected = false;
		m_is_on_pickup = false;
		new_mode = NONE;
	}


	bool DetectorPoint::CheckForCollisions()
	{
		if (m_obstacles.size() == 0)
			return false;
		
		// Precalculate the rounding down and conversion to int.
		int i_pre_pos_x{ (int)floor(prev_position.x) },
			i_pre_pos_y{ (int)floor(prev_position.y) },
			i_pos_x{ (int)floor(position.x) },
			i_pos_y{ (int)floor(position.y) };

		/*
			Use the previous position and current position
			to calculate the line that was traveled.

			Components of the line's slope. If either is zero,
			assign values based on special case. Otherwise,
			calculate slope and y-intercept to get values.

			Use standard form: Ax + By + C = 0.
		*/

		int rise = (i_pre_pos_y - i_pos_y);
		int run = (i_pre_pos_x - i_pos_x);

		StandardForm line1{};

		if (rise == 0)
		{
			line1.SetVars(0.0f, 1.0f, static_cast<float>(-i_pos_y));
		}
		else if (run == 0)
		{
			line1.SetVars(1.0f, 0.0f, static_cast<float>(-i_pos_x));
		}
		else
		{
			// Y = mX + B
			float m{ static_cast<float>(1.0f * rise / run) };
			float B{ i_pos_y - (m * i_pos_x) };

			line1.SetVars(-m, 1.0f, -B);
		}

		/*
			For each obstacle the player's collision box
			intersected, check if the path of the player's
			movement crossed any of it's edges.

			Store all intersection points for later comparisons.
		*/

		std::vector<Intersection> intersections;

		for (Collidable*& obstacle : m_obstacles)
		{
			if (obstacle == nullptr) continue;

			bool obstacle_found{ false };

			if (obstacle->top >= top && obstacle->top <= bottom)
			{
				obstacle_found = true;
				CheckForIntersection(intersections, line1, obstacle, EdgeCrossed::TOP);
			}

			if (obstacle->bottom >= top && obstacle->bottom <= bottom)
			{
				obstacle_found = true;
				CheckForIntersection(intersections, line1, obstacle, EdgeCrossed::BOTTOM);
			}

			if (obstacle->left >= left && obstacle->left <= right)
			{
				obstacle_found = true;
				CheckForIntersection(intersections, line1, obstacle, EdgeCrossed::LEFT);
			}

			if (obstacle->right >= left && obstacle->right <= right)
			{
				obstacle_found = true;
				CheckForIntersection(intersections, line1, obstacle, EdgeCrossed::RIGHT);
			}

			if (!obstacle_found)
			{
				if (i_pos_x >= obstacle->left && i_pos_x <= obstacle->right
					&& i_pos_y >= obstacle->top && i_pos_y <= obstacle->bottom)
				{
					printf("Currently in object but not detected!");
				}
			}
		}


		/*
			Now find the intersection that is closest to the previous center.
			This will be the first obstacle encountered and the one that
			will be processed. The rest are discarded.
		*/

		Intersection closest_intersection{};
		if (intersections.size() > 0)
		{
			// First find intersection points nearest to previous position
			// This will tell us what was hit first, before anything else.
			float closest_distance{ 1024.0f };
			for (Intersection& intersection : intersections)
			{
				float dist_x{ intersection.m_intersection.x - i_pre_pos_x };
				float dist_y{ intersection.m_intersection.y - i_pre_pos_y };
				float distance{ (dist_x * dist_x) + (dist_y * dist_y) };
				if (distance <= closest_distance)
				{
					closest_intersection = intersection;
					closest_distance = distance;
				}
			}
		}		

		/*
			Move player center to hit, and add offset based on side
			hit so player is not in obstacle.
		*/

		
		if (closest_intersection.m_collision_object)
		{
			/*
				Find the distance needed to move to collision point.
				Then move 1 additional unit away from the edge to prevent another collision.
			*/
			delta_position.x = closest_intersection.m_intersection.x - i_pos_x;
			delta_position.y = closest_intersection.m_intersection.y - i_pos_y;
			delta_position += GetCollisionBuffer(closest_intersection.m_collision_edge_crossed);


			switch (closest_intersection.m_type_of_collision_object)
			{
			case CollidableType::OBSTACLE:
			case CollidableType::PERM_GLOW:
			case CollidableType::FINISH_LINE:
				ResolveSmoothCollision(closest_intersection);
				break;
			default:
				break;
			}

			m_is_collision_detected = true;
		}		
		return m_is_collision_detected;
	}


	void DetectorPoint::CheckForIntersection(std::vector<Intersection>& intersects, StandardForm& line1, Collidable*& collision_object, EdgeCrossed collision_edge_crossed)
	{
		/* 
			Compare the standard form of the lines in question to see if an intersection is possible, then find the intersection point,
			and then confirm the intersection is within the bounds of the object before storing it as a valid intersection.
		*/
		
		StandardForm line2{};
		ConvertLineToStandardForm(collision_object, line2, collision_edge_crossed);

		if (GetIsCollisionPossible(line1, line2))
		{
			vec2 intersection{ GetIntersection(line1, line2) };

			if (GetIsIntersectionInBounds(intersection, collision_object))
			{
				int penetration{ GetPenetrationDepth(collision_object, collision_edge_crossed) };
				intersects.push_back(Intersection{ collision_object, intersection, collision_edge_crossed, penetration });
			}
		}
	}

	void DetectorPoint::ConvertLineToStandardForm(Collidable*& collision_object, StandardForm& std_form, EdgeCrossed& collision_edge_crossed)
	{
		/* Sets the Ax + By + C = 0 standard form variables for the edge. All cases are special (vertical and horizontal lines). */

		switch (collision_edge_crossed)
		{
		case EdgeCrossed::LEFT:
			std_form.SetVars(1.0f, 0.0f, static_cast<float>(-collision_object->left));
			break;
		case EdgeCrossed::RIGHT:
			std_form.SetVars(1.0f, 0.0f, static_cast<float>(-collision_object->right));
			break;
		case EdgeCrossed::TOP:
			std_form.SetVars(0.0f, 1.0f, static_cast<float>(-collision_object->top));
			break;
		case EdgeCrossed::BOTTOM:
			std_form.SetVars(0.0f, 1.0f, static_cast<float>(-collision_object->bottom));
			break;
		}
	}


	bool DetectorPoint::GetIsCollisionPossible(StandardForm& line1, StandardForm& line2)
	{
		/* If value of same var are both 0, lines will not intersect. */

		return ((line1.A != 0.0f || line2.A != 0.0f)
			&& (line1.B != 0.0f || line2.B != 0.0f));
	}


	vec2 DetectorPoint::GetIntersection(StandardForm& line1, StandardForm& line2)
	{
		/* Equation to find intersection of 2 lines in standard form. Could not find technical name for it. */

		return vec2{
			((line1.B * line2.C) - (line2.B * line1.C)) / ((line1.A * line2.B) - (line2.A * line1.B)),
			((line1.C * line2.A) - (line2.C * line1.A)) / ((line1.A * line2.B) - (line2.A * line1.B))
		};
	}


	int DetectorPoint::GetPenetrationDepth(Collidable*& collision_object, EdgeCrossed collision_edge_crossed)
	{
		switch (collision_edge_crossed)
		{
		case EdgeCrossed::LEFT:
			return abs(collision_object->left - (int)floor(position.x));
		case EdgeCrossed::RIGHT:
			return abs(collision_object->right - (int)floor(position.x));
		case EdgeCrossed::TOP:
			return abs(collision_object->top - (int)floor(position.y));
		case EdgeCrossed::BOTTOM:
			return abs(collision_object->bottom - (int)floor(position.y));
		}
	}


	bool DetectorPoint::GetIsIntersectionInBounds(vec2& intersection, Collidable*& collision_object)
	{
		/* 
			Make sure intersection is within bounds of collision object. Just because their collision
			boxes overlap does not mean their actual lines intersect on the object.
		*/

		return (intersection.x >= collision_object->left && intersection.x <= collision_object->right
			&& intersection.y >= collision_object->top && intersection.y <= collision_object->bottom);
	}


	void DetectorPoint::ResolveSmoothCollision(Intersection& intersection_info)
	{
		/*
			Get next mode or ricochet velocity (no mode if ricochet).
		*/

		isRicochetCollision = GetIsRicochetCollision(intersection_info.m_collision_edge_crossed);

		if (isRicochetCollision)
		{
			SetRicochetSpeed(intersection_info);
		}
		else
		{
			new_mode = GetNextMode();
			m_is_safe_glow_needed = GetIsSafeGlowNeeded();
		}
	}


	bool DetectorPoint::GetIsRicochetCollision(EdgeCrossed& collision_edge_crossed)
	{
		/*
			The cardinal points only ricochet if they don't make a collision opposing
			their primary direction. If the right point hits the left edge of an object,
			that is a regular collision. If the right point hits the top edge of an object,
			that results in a ricochet.

			Normal collision	Ricochet collision

				+-----			O
			O > |				V
				|				+--
				|				|

			The angled points will always ricochet if they create the closest collision.
		*/

		switch (post_id)
		{
		case RIGHT:
			return (collision_edge_crossed != EdgeCrossed::LEFT);
		case TOP:
			return (collision_edge_crossed != EdgeCrossed::BOTTOM);
		case LEFT:
			return (collision_edge_crossed != EdgeCrossed::RIGHT);
		case BOTTOM:
			return (collision_edge_crossed != EdgeCrossed::TOP);
		default:
			return true;
		}
	}


	int DetectorPoint::GetNextMode()
	{
		/*
			Hits on the cardinal points will put the player in a new mode.
		*/

		switch (post_id)
		{
		case RIGHT:
			return WALL;
		case TOP:
			return CEILING;
		case LEFT:
			return WALL;
		case BOTTOM:
			return GROUND;
		default:
			return NONE; // Shouldn't reach.
		}
	}


	bool DetectorPoint::GetIsSafeGlowNeeded()
	{
		// If none of the safe glow orbs are overlapping the point, a new safe glow orb is needed.
		for (Collidable*& safe_orb : m_glow_orbs)
		{
			// Permanent glow blocks act as a safe glow orb.
			if (safe_orb->m_object_type == CollidableType::PERM_GLOW) return false;

			float dist_x{ position.x - safe_orb->center.x };
			float dist_y{ position.y - safe_orb->center.y };
			float dist = (dist_x * dist_x) + (dist_y * dist_y);
			float radius = safe_orb->center.x - safe_orb->left;
			if (dist <= radius * radius)
			{
				return false;
			}
		}
		return true;
	}

	
	void DetectorPoint::SetRicochetSpeed(Intersection& intersection_info)
	{
		/*
			Ricochet based on what edge of the collision object was crossed.
			Left/Right dampens and reverses x velocity, and slighty dampens y velocity.
			Top/Bottom dampens and reverses y velocity with a small decrease to x velocity.
		*/

		switch (intersection_info.m_collision_edge_crossed)
		{
		case EdgeCrossed::LEFT:
			velocity.y = m_state == State::ALIVE ? 3.0f : 1.0f;
			break;
		case EdgeCrossed::RIGHT:
			velocity.y = m_state == State::ALIVE ? 3.0f : 1.0f;
			break;
		case EdgeCrossed::TOP:
			velocity.x = (fabsf(velocity.x) + 15.0f);
			velocity.y = m_state == State::ALIVE ? -2.0f : -1.0f;
			break;
		case EdgeCrossed::BOTTOM:
			velocity.x = (fabsf(velocity.x) + 15.0f);
			velocity.y = m_state == State::ALIVE ? 2.0f : 1.0f;
			break;
		}

		velocity.x *= intersection_info.GetHorizontalRicochetDirection();
	}


	vec2 DetectorPoint::GetCollisionBuffer(EdgeCrossed collision_edge_crossed)
	{
		switch (collision_edge_crossed)
		{
		case EdgeCrossed::LEFT:
			return vec2{ -1.0f, 0.0f };
		case EdgeCrossed::RIGHT:
			return vec2{ 1.0f, 0.0f };
		case EdgeCrossed::TOP:
			return vec2{ 0.0f, -1.0f};
		case EdgeCrossed::BOTTOM:
			return vec2{ 0.0f, 1.0f };
		}
	}
};