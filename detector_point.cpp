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
	/* Neighbor obstacle directions. */
	constexpr int TOP_NEIGHBOR{ 1 };
	constexpr int LEFT_NEIGHBOR{ 2 };
	constexpr int RIGHT_NEIGHBOR{ 4 };
	constexpr int BOTTOM_NEIGHBOR{ 8 };

	DetectorPoint::DetectorPoint(int assigned_post) :
		Collidable{ CollidableInfo{CollidableType::PLAYER_POINT, CollisionLayer::PLAYER, CollisionMask::PLAYER, 0} },
		post_id{ assigned_post }
	{	}


	void DetectorPoint::SetPosition(vec2& m_center, int radius)
	{
		float deg45{ (2 * PI) / 8 };

		position.x = m_center.x + radius * cos(deg45 * post_id);
		position.y = m_center.y - radius * sin(deg45 * post_id);
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


	vec2& DetectorPoint::GetDeltaPosition()
	{
		return m_delta_position;
	}


	int DetectorPoint::GetNewMode()
	{
		return m_new_mode;
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
		m_delta_position.x = 0.0f;
		m_delta_position.y = 0.0f;
		m_is_collision_detected = false;
		m_is_on_pickup = false;
		m_is_tethered = false;
		m_tethered_object = nullptr;
		m_new_mode = NONE;
	}


	void DetectorPoint::RegisterCollision(Collidable*& collision)
	{
		switch (collision->m_collidable_type)
		{
		case CollidableType::OBSTACLE_HIDDEN:
		case CollidableType::OBSTACLE_DANGEROUS:		
		case CollidableType::OBSTACLE_MOVING_HIDDEN:
			m_obstacles.push_back(collision);
			break;
		case CollidableType::GLOW_ORB_SAFE:
			m_glow_orbs.push_back(collision);
			break;
		case CollidableType::OBSTACLE_VISIBLE:
		case CollidableType::OBSTACLE_MOVING_VISIBLE:
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
					// The moving obstacle has hit the player while the player had no movement on the x axis.
					// The detector point cannot detect things on an axis it is not moving on.
					// We will create an approximate interesection.
					// The player always has y movement, so this is only an issue on the x axis.
					if (obstacle->m_collidable_type == CollidableType::OBSTACLE_MOVING_VISIBLE
						|| obstacle->m_collidable_type == CollidableType::OBSTACLE_MOVING_HIDDEN)
					{
						switch (post_id)
						{
						case LEFT: // Make an intersection on the right edge of the obstacle.
							intersections.emplace_back(
								obstacle,
								vec2{ static_cast<float>(obstacle->right), position.y },
								EdgeCrossed::RIGHT,
								(obstacle->right - i_pos_x)
							);
							break;
						case RIGHT: // Make an intersection on the left edge of the obstacle.
							intersections.emplace_back(
								obstacle,
								vec2{ static_cast<float>(obstacle->left), position.y },
								EdgeCrossed::LEFT,
								(i_pos_x - obstacle->left)
							);
							break;
						default:
							printf("Currently in moving object but not detected!");
							break;
						}
					}
					else
					{
						printf("Currently in unknown object but not detected!");
					}					
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
			m_delta_position.x = closest_intersection.m_intersection.x - i_pos_x;
			m_delta_position.y = closest_intersection.m_intersection.y - i_pos_y;
			m_delta_position += GetCollisionBuffer(closest_intersection.m_collision_edge_crossed);

			ResolveSmoothCollision(closest_intersection);

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

		m_is_ricochet_collisions = GetIsRicochetCollision(intersection_info.m_collision_edge_crossed, intersection_info.m_collision_object->m_neighbors);

		if (m_is_ricochet_collisions)
		{
			SetRicochetSpeed(intersection_info);
		}
		else
		{
			DetermineNextMode();
			DetermineIsSafeGlowNeeded();			
			DetermineIsTethered(intersection_info);
			DetermineIsOnDangerousObstacle(intersection_info);
		}
	}


	bool DetectorPoint::GetIsRicochetCollision(EdgeCrossed& collision_edge_crossed, int neighbors)
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

			The angled points (1, 3, 5, & 7) will always ricochet if they create the closest collision.
		*/

		// If edge in question is adjacent to another obstacle, it cannot have provided a corner to ricochet off of.
		switch (collision_edge_crossed)
		{
		case EdgeCrossed::TOP:
			if (neighbors & TOP_NEIGHBOR)
				return false;
		case EdgeCrossed::LEFT:
			if (neighbors & LEFT_NEIGHBOR)
				return false;
		case EdgeCrossed::RIGHT:
			if (neighbors & RIGHT_NEIGHBOR)
				return false;
		case EdgeCrossed::BOTTOM:
			if (neighbors & BOTTOM_NEIGHBOR)
				return false;
		default:
			break;
		}

		// Otherwise, check if the cardinal point did not hit an opposing edge. That makes a ricochet.
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
			velocity.x = 10.0f;
			velocity.y = m_state == State::ALIVE ? -2.0f : -1.0f;
			break;
		case EdgeCrossed::BOTTOM:
			velocity.x = (fabsf(velocity.x) + 15.0f);
			velocity.y = m_state == State::ALIVE ? 2.0f : 1.0f;
			break;
		}

		velocity.x *= intersection_info.GetHorizontalRicochetDirection();
	}


	void DetectorPoint::DetermineNextMode()
	{
		/*
			Hits on the cardinal points will put the player in a new mode.
		*/

		
		switch (post_id)
		{
		case RIGHT:
			m_new_mode = WALL;
			break;
		case TOP:
			m_new_mode = CEILING;
			break;
		case LEFT:
			m_new_mode = WALL;
			break;
		case BOTTOM:
			m_new_mode = GROUND;
			break;
		default:
			m_new_mode = NONE; // Shouldn't reach.
			break;
		}
	}

		
	void DetectorPoint::DetermineIsSafeGlowNeeded()
	{
		// If none of the safe glow orbs are overlapping the point, a new safe glow orb is needed.
		for (Collidable*& safe_orb : m_glow_orbs)
		{
			// Visible obstacles act as a safe glow orb.
			if (safe_orb->m_collidable_type == CollidableType::OBSTACLE_VISIBLE
				|| safe_orb->m_collidable_type == CollidableType::OBSTACLE_MOVING_VISIBLE)
			{
				m_is_safe_glow_needed = false;
				return;
			}

			float dist_x{ position.x - safe_orb->m_center.x };
			float dist_y{ position.y - safe_orb->m_center.y };
			float dist = (dist_x * dist_x) + (dist_y * dist_y);
			float radius = safe_orb->m_center.x - safe_orb->left;
			if (dist <= radius * radius)
			{
				m_is_safe_glow_needed = false;
				return;
			}
		}

		m_is_safe_glow_needed = true;
	}


	void DetectorPoint::DetermineIsTethered(Intersection& intersection_info)
	{
		if (intersection_info.m_collision_object->m_collidable_type == CollidableType::OBSTACLE_MOVING_VISIBLE
			|| intersection_info.m_collision_object->m_collidable_type == CollidableType::OBSTACLE_MOVING_HIDDEN)
		{
			m_is_tethered = true;
			m_tethered_object = intersection_info.m_collision_object;
		}
	}


	void DetectorPoint::DetermineIsOnDangerousObstacle(Intersection& intersection_info)
	{
		m_is_on_dangerous_obstacle = intersection_info.m_collision_object->m_collidable_type == CollidableType::OBSTACLE_DANGEROUS;
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