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
	/* Bitwise values of surface types. */
	constexpr int SMOOTH{ 1 };
	constexpr int SMOOTH_CORNER{ 2 };
	constexpr int ROUGH{ 4 };
	constexpr int ROUGH_CORNER{ 8 };


	DetectorPoint::DetectorPoint(int assigned_post) :
		post{ assigned_post }
	{	
		m_object_type = CollidableType::PLAYER;
	}


	void DetectorPoint::SetPosition(vec2& center, int radius)
	{
		float deg45{ (2 * PI) / 8 };

		position.x = center.x + radius * cos(deg45 * post);
		position.y = center.y - radius * sin(deg45 * post);
		prev_position = position;

		UpdateCollisionBox();
	}


	void DetectorPoint::UpdatePosition(vec2& player_speed)
	{
		speed = player_speed;

		prev_position = position;
		position += speed;
				
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


	void DetectorPoint::ResolveCollision(Collidable*& collision)
	{
		collisions.push_back(collision);
	}


	vec2& DetectorPoint::GetDeltaPosition()
	{
		return delta_position;
	}


	int DetectorPoint::GetNewMode()
	{
		return new_mode;
	}


	vec2& DetectorPoint::GetNewSpeed()
	{
		return speed;
	}


	int DetectorPoint::GetCollisionPointBinary()
	{
		return (post == 0 ? 0 : 1 << (post - 1));
	}


	void DetectorPoint::ApplyDeltaPosition(vec2& delta_position)
	{
		if (collisions.size() > 0)
			collisions.clear();

		if (post == 6)
		{
			printf("before delta: %f, %f\n", position.x, position.y);
			printf("delta: %f, %f\n", delta_position.x, delta_position.y);
		}

		position += delta_position;		

		if (post == 6)
		{
			printf("after delta: %f, %f\n", position.x, position.y);
			printf("\n");
		}

		UpdateCollisionBox();
	}

	void DetectorPoint::UpdatePreviousPosition()
	{
		/*
			Needed for when player enters non-AIR state and stops updating position.
			If not done, prev position may stay inside an object.	
		*/
		prev_position = position;
	}


	bool debug_mode{ false };

	bool DetectorPoint::CheckForCollisions()
	{
		delta_position.x = 0.0f;
		delta_position.y = 0.0f;
		new_mode = NONE;

		if (collisions.size() == 0)
			return false;
		
		debug_mode = false;

		if (post == 6)
		{
			printf("prev pos: %f, %f\n", prev_position.x, prev_position.y);
			printf("landing pos: %f, %f\n", position.x, position.y);
			debug_mode = true;
		}

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

		for (Collidable*& collision : collisions)
		{
			bool collision_found{ false };

			if (collision->top >= top && collision->top <= bottom)
			{
				collision_found = true;
				CheckForIntersection(intersections, line1, collision, EdgeCrossed::TOP);
			}

			if (collision->bottom >= top && collision->bottom <= bottom)
			{
				collision_found = true;
				// Bottom edge of obstacle was crossed.
				CheckForIntersection(intersections, line1, collision, EdgeCrossed::BOTTOM);
			}

			if (collision->left >= left && collision->left <= right)
			{
				collision_found = true;
				// Left edge of obstacle was crossed.
				CheckForIntersection(intersections, line1, collision, EdgeCrossed::LEFT);
			}

			if (collision->right >= left && collision->right <= right)
			{
				collision_found = true;
				// Right edge of obstacle was crossed.
				CheckForIntersection(intersections, line1, collision, EdgeCrossed::RIGHT);
			}

			if (!collision_found)
			{
				if (i_pos_x >= collision->left && i_pos_x <= collision->right
					&& i_pos_y >= collision->top && i_pos_y <= collision->bottom)
				{
					printf("Currently in object but not detected!");
				}
			}
		}

		// Clear collisions for next step.
		//collisions.clear();

		/*
			Now find the intersection that is closest to the previous center.
			The will be the first collision encountered and the one that
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
			case CollidableType::SMOOTH:
			case CollidableType::ROUGH:	// for testing.
				ResolveSmoothCollision(closest_intersection);
				break;
			default:
				break;
			}

			return true;
		}
		else
		{
			return false;
		}
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
				if (debug_mode)
					printf("edge crossed: %d - ok\n", collision_edge_crossed);

				int penetration{ GetPenetrationDepth(collision_object, collision_edge_crossed) };
				intersects.push_back(Intersection{ collision_object, intersection, collision_edge_crossed, penetration });
			}
			else
			{
				if (debug_mode)
					printf("edge crossed: %d - out of bounds\n", collision_edge_crossed);
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
			Get next mode or ricochet speed (no mode if ricochet).
		*/

		isRicochetCollision = GetIsRicochetCollision(intersection_info.m_collision_edge_crossed);

		if (isRicochetCollision)
		{
			SetRicochetSpeed(intersection_info);
		}
		else
		{
			new_mode = GetNextMode();
		}
	}


	constexpr int DetectorPoint::GetNextMode()
	{
		/*
			Hits on the cardinal points will put the player in a new mode.
		*/

		switch (post)
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


	bool DetectorPoint::GetIsRicochetCollision(EdgeCrossed& collision_edge_crossed)
	{
		/*
			The cardinal points only ricochet if they don't make a collision opposing
			their primary direction. If the right point hits the left edge of an object,
			that is a regular collision. If the right point hits the top edge of an object,
			that results in a ricochet.

			Normal collision	Ricochet collision

				+-----			 O	
			O > |				 V
				|				+---
				|				|
			
			The angled points will always ricochet if they create the closest collision.
		*/


		switch (post)
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


	void DetectorPoint::ResolveRoughCollision()
	{
	}


	void DetectorPoint::ResolveRoughCornerCollision()
	{
	}


	void DetectorPoint::SetRicochetSpeed(Intersection& intersection_info)
	{
		/*
			Ricochet based on what edge of the collision object was crossed.
			Left/Right dampens and reverses x speed, and slighty dampens y speed.
			Top/Bottom dampens and reverses y speed with a small decrease to x speed.
		*/

		switch (intersection_info.m_collision_edge_crossed)
		{
		case EdgeCrossed::LEFT:
			speed.x = fabsf(speed.x * 0.5f);
			speed.y *= 0.75f;
			break;
		case EdgeCrossed::RIGHT:
			speed.x = fabsf(speed.x * 0.5f);
			speed.y *= 0.75f;
			break;
		case EdgeCrossed::TOP:
			speed.x = (fabsf(speed.x) + 1.5f);
			speed.y = -2.0f;
			break;
		case EdgeCrossed::BOTTOM:
			speed.x = (fabsf(speed.x) + 1.5f);
			speed.y = 1.0f;
			break;
		}

		speed.x *= intersection_info.GetHorizontalRicochetDirection();
	}


	vec2 DetectorPoint::GetCollisionBuffer(EdgeCrossed collision_edge_crossed)
	{
		if (debug_mode)
			printf("buffer for: %d\n", collision_edge_crossed);

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