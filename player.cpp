#pragma once

#include <algorithm>
#include "player.h"

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
	/* Bitwise values of surface types. */
	constexpr int SMOOTH{ 1 };
	constexpr int SMOOTH_CORNER{ 2 };
	constexpr int ROUGH{ 4 };
	constexpr int ROUGH_CORNER{ 8 };


	// Constructor.
	Player::Player(Surface* screen, keyState& leftKey, keyState& rightKey, keyState& upKey, keyState& downKey) :
		m_screen{ screen },
		m_sprite{ Sprite{new Surface("assets/ball.png"), 3, true} }, // player.png is 40px by 40px.
		half_height{ m_sprite.GetHeight() / 2 },
		half_width{ m_sprite.GetWidth() / 2 },
		half_size{ half_height },
		m_leftKey{ leftKey }, m_rightKey{ rightKey }, m_upKey{ upKey }, m_downKey{ downKey }
	{
		// Create array of points that will go around circle.
		for (int i{ 0 }; i < POINTS; i++)
		{
			points.push_back(DetectorPoint{ i });
		}
	}


	// -----------------------------------------------------------
	// Public methods.
	// -----------------------------------------------------------

	void Player::Update(float deltaTime)
	{
		m_delta_time = deltaTime;

		/* Update position and draw sprite. */

		switch (mode)
		{
		case Mode::AIR:
			updateAir();
			break;
		case Mode::GROUND:
			updateGround();
			break;
		case Mode::WALL:
			updateWall();
			break;
		case Mode::CEILING:
			updateCeiling();
			break;
		case Mode::REST:
			break;
		case Mode::NONE:
		default:
			updateAir();
			break;
		}
	}


	void Player::Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		for (DetectorPoint& point : points)
		{
			viewable_layer->Box(point.left - c_left, point.top - c_top, point.right - c_left, point.bottom - c_top, 0xFFFFFFFF);
		}

		m_sprite.Draw(viewable_layer, position.x - c_left, position.y - c_top);
	}


	void Player::SetPosition(vec2& start_position)
	{
		position = start_position;
		SetCenterAndBounds();

		for (DetectorPoint& point : points)
		{
			point.SetPosition(center, half_size - 4);
		}
	}


	void Player::UpdatePosition()
	{
		prev_position = position;

		float half_t2{ 0.5f * m_delta_time * m_delta_time };

		// If player isn't pressing left/right and we have no velocity, don't move.
		distance.x = (direction.x == 0.0f && velocity.x == 0.0f) ? 0.0f : GetDistanceToMove(0, half_t2);
		distance.y = GetDistanceToMove(1, half_t2);

		position.x += distance.x;
		position.y += distance.y;

		SetCenterAndBounds();

		for (DetectorPoint& point : points)
		{
			point.UpdatePosition(velocity, distance);
		}
	}


	float Player::GetDistanceToMove(int vec2_index, float pre_calculated_half_t2)
	{
		// distance = velocity * time + 1/2 * acceleration * time^2.
		return ((velocity[vec2_index] * m_delta_time) + (velocity[vec2_index] * pre_calculated_half_t2))
			// mutliplied so acceleration and velocity can stay as manageable numbers.
			* magnitude_coefficient[vec2_index];			
	}


	float Player::GetDistanceToBounceApex()
	{
		float time_to_apex{ ground_bounce_power / acceleration.y };
		float distance_of_apex{ (ground_bounce_power * time_to_apex) + (0.5f * acceleration.y * time_to_apex * time_to_apex) };
		return distance_of_apex * magnitude_coefficient.y;
	}


	void Player::SetCenterAndBounds()	
	{
		center = vec2(position.x + half_width, position.y + half_height);
		UpdateCollisionBox();
	}


	void Player::UpdateCollisionBox()
	{
		// We are in pixel-space and will only use whole numbers.

		left = (int)floor(position.x);
		right = (int)floor(left + m_sprite.GetWidth());
		top = (int)floor(position.y);
		bottom = (int)floor(top + m_sprite.GetHeight());
	}


	std::vector<DetectorPoint>& Player::GetCollisionPoints()
	{	
		return points;
	}

	
	void Player::RegisterGlowSocket(Socket<GlowMessage>& glow_socket)
	{
		m_glow_socket = &glow_socket;
	}


	void Player::RegisterCameraSocket(Socket<CameraMessage>& camera_socket)
	{
		m_camera_socket = &camera_socket;
	}


	void Player::ResolveCollision(Collidable*& collision)
	{
		collisions.push_back(collision);
	}


	void Player::ProcessCollisions()
	{
		int new_mode{ NONE };
		bool is_ricochet_set{ false };
		int post_id{ NONE };
		vec2 delta_position{ 0.0f, 0.0f };
		vec2 ricochet_velocity{ 0.0f, 0.0f };

		// Get change in position after collision, and the new mode.
		for (DetectorPoint& point : points)		
		{			
			if (point.CheckForCollisions())
			{
				/*
					Update delta position (shifting out of collision object).
					Only keep the largest changes to x and y.
				*/				

				vec2 point_delta_position{ point.GetDeltaPosition() };				
				delta_position.x = GetAbsoluteMax(point_delta_position.x, delta_position.x);
				delta_position.y = GetAbsoluteMax(point_delta_position.y, delta_position.y);

				/*
					Get new mode (hit a flat surface). If new mode is WALL,
					store the post info so we know which side of player the wall is.
				*/
				new_mode |= point.GetNewMode();
				if (point.GetNewMode() & WALL)
				{
					post_id = point.post_id;
				}

				// If a ricochet collision, store the first instance.
				if (point.isRicochetCollision && !is_ricochet_set)
				{
					ricochet_velocity = point.GetNewVelocity();
					is_ricochet_set = true;
				}
			}
		}
		

		// Apply the change in position to all points and player.
		// Increase the delta x and y by 1 in the given direction if not 0.
		/*delta_position.x += GetSign(delta_position.x);
		delta_position.y += GetSign(delta_position.y);*/

		if (delta_position.x != 0.0f || delta_position.y != 0.0f) // If at least 1 axis is not 0, there was a collision that needs to be handled.
		{
			position += delta_position;
			SetCenterAndBounds();
			for (DetectorPoint& point : points)
			{
				point.ApplyDeltaPosition(delta_position);
				point.ClearCollisions();
			}


			// Update mode. If no mode, check if ricochet and apply new velocity.
			bool is_safe_glow_needed{ false };
			if (new_mode & ~NONE)
			{
				if (new_mode & GROUND)
				{
					handleGroundCollision();
					is_safe_glow_needed = GetIsSafeGlowNeeded(BOTTOM);
				}
				else if (new_mode & WALL)
				{
					handleWallCollision(post_id);
					is_safe_glow_needed = GetIsSafeGlowNeeded(post_id);
				}
				else
				{
					handleCeilingCollision();
					is_safe_glow_needed = GetIsSafeGlowNeeded(TOP);
				}
			}
			else if (is_ricochet_set)
			{
				velocity = ricochet_velocity;
			}

			if (is_safe_glow_needed)
			{
				--m_player_strength;
			}

			// Update glow socket.
			if (new_mode & ~NONE)
			{				
				m_glow_socket->SendMessage(GlowMessage{ center, m_player_strength/m_player_max_strength, CollidableType::FULL_GLOW, is_safe_glow_needed });
			}
			else if (is_ricochet_set)
			{
				m_glow_socket->SendMessage(GlowMessage{ center, m_player_strength / m_player_max_strength, CollidableType::TEMP_GLOW });
			}
		}
		else
		{
			for (DetectorPoint & point : points)
			{
				point.ClearCollisions();
			}
		}
	}


	/*
		Credit to user79785: https://stackoverflow.com/questions/1903954/is-there-a-standard-sign-function-signum-sgn-in-c-c
	*/
	template <typename T>
	int Player::GetSign(T val) {
		return (T(0) < val) - (val < T(0));
	}

	template <typename T>
	T Player::GetAbsoluteMax(T val1, T val2) {
		return (abs(val1) > abs(val2) ? val1 : val2);
	}


	bool Player::GetIsSafeGlowNeeded(int post_id)
	{
		return points[post_id].m_is_safe_glow_needed;
	}


	/*
		This section covers actions while in the air (or about to collide).
	*/


	void Player::updateAir()
	{
		/* Handles movement. */

		updateVerticalMovement();
		updateHorizontalMovement();
		UpdatePosition();
	}


	void Player::updateVerticalMovement()
	{
		/* Check if sprite image can be changed to normal. Stretched image is
			immediate result of squashed/GROUND state. Update vertical movement
			and velocity.y, keeping within bounds. */

		updateFrameStretch2Normal();

		// V.final = V.initial + (acceleration * time);
		velocity.y += acceleration.y * m_delta_time;
	}


	void Player::updateHorizontalMovement()
	{
		/* Update horizontal position, unless direction locked from weak wall bounce. */

		direction.x = -(m_leftKey.isActive) + m_rightKey.isActive;
		if (direction.x != 0)
		{
			if (mode == Mode::AIR)
			{
				velocity.x += direction.x * acceleration.x * m_delta_time;
				velocity.x = Clamp(velocity.x, -max_velocity.x, max_velocity.x);
			}
		}
		else
		{
			if (velocity.x != 0.0f)
			{
				if (fabsf(velocity.x) < 0.3f) //dead_zone.x;
				{
					velocity.x = 0.0f;
				}
				else if (mode == Mode::AIR)
				{
					velocity.x += GetSign(velocity.x) * -1 * acceleration.x * m_delta_time;
					velocity.x = Clamp(velocity.x, -max_velocity.x, max_velocity.x);
				}
			}
		}
	}


	void Player::handleGroundCollision()
	{
		/* Cheat the y position so the ball can appear to be above the ground.
			Determine if ball will be squashed or come to a complete rest. */

		// Set next mode.
		if (deadZone > fabsf(velocity.y))
		{
			stopBouncing();
		}
		else
		{
			prepareForGroundMode();
		}

		m_camera_socket->SendMessage(CameraMessage{ center, Location::GROUND });

		// Set next mode.
		mode = Mode::GROUND;
	}


	void Player::stopBouncing()
	{
		/* Lose all vertical velocity and set to normal image. REST mode is a dead end. */

		velocity.y = 0.0f;
		m_sprite.SetFrame(0);
		mode = Mode::REST;
	}


	void Player::prepareForGroundMode()
	{
		/* Remove any directional lock since player can't move ball while it is
			squashed anyway. Calculate the squash and stretch frame counts. Slower
			speeds result in less frames of each. GROUND mode squashes the ball. */

		setSquashFrameCount();
		setStretchFrameCount();

		setFrameNormal2Squash();

		// Update positions. Brings them above ground after collision.
		for (DetectorPoint& point : points)
		{
			point.UpdatePreviousPosition();
		}
	}


	void Player::setSquashFrameCount()
	{
		/* Squash delay / animation is a function of impact velocity. All in an effort
			to reduce / remove squashing when at low speeds. Around 4 velocity there
			should be no squashing at all, but kept it generalized just in case. */

		// Easier to step through when spaced out.
		float squashValue = fabsf(velocity.y);
		squashValue -= squashDampeningMagnitude;
		squashValue *= squashDampeningCoefficient;
		squashValue = ceil(squashValue) - 1;

		printf("squash sec: %f\n", squashValue);
		m_squash_frame_seconds = 0.5f; //static_cast<int>(squashValue);
	}


	void Player::setStretchFrameCount()
	{
		/* Stretch animation is a function of squash delay. Must be calculated
			before the squash frames begin counting down. */

		stretchFrameCount = m_squash_frame_seconds;
	}


	void Player::handleWallCollision(int post_id)
	{
		/* Based on wall, prepare for WALL mode. Frame int is based on 3 images per
			orientation and style of ball. */

		bool isOnLeftWall = (post_id == LEFT);

		if (isOnLeftWall)
			prepareForWallMode(Trigger::RIGHT);
		else
			prepareForWallMode(Trigger::LEFT);

		m_leftKey.isActive = false;
		m_rightKey.isActive = false;
		m_upKey.isActive = false;
		m_downKey.isActive = false;

		m_camera_socket->SendMessage(CameraMessage{ center, Location::WALL });

		// Set next mode.
		mode = Mode::WALL;
	}


	void Player::prepareForWallMode(Trigger trigger)
	{
		/* Reposition ball just off of wall and lose all velocity. Register trigger and
			set trigger duration. Update the sprite. WALL mode sticks ball on wall. */

		velocity.x = 0.0f;

		wallBounceTrigger = trigger;
		triggerFrameCount = 1.0f;
		
		// Need to figure out how to rotate sprite for side squash. Matrix rotation preferred.
		// But can also create static sprite frames for each wall and ceiling.
		//m_sprite.SetFrame(newFrame);
	}


	void Player::handleCeilingCollision()
	{
		/* Based on wall, prepare for WALL mode. Frame int is based on 3 images per
			orientation and style of ball. */

		prepareForCeilingMode();

		m_camera_socket->SendMessage(CameraMessage{ center, Location::OTHER });

		// Set next mode.
		mode = Mode::CEILING;
	}


	void Player::prepareForCeilingMode()
	{
		/* Remove any directional lock since player can't move ball while it is
			squashed anyway. Calculate the squash and stretch frame counts. Slower
			speeds result in less frames of each. GROUND mode squashes the ball. */

		velocity.y = 0.0f;

		setSquashFrameCount();
		setStretchFrameCount();

		setFrameNormal2Squash();
	}


	/*
		This section covers when the ball hits the ground and bounces back up.
	*/


	void Player::updateGround()
	{
		/* "Freeze" ball in squash mode for squash frame count. Then bounce back up
			with equal velocity to landing velocity. Reset max_velocity.x to normal velocity (is
			increased after a strong wall bounce). AIR mode handles movement and
			collisions. */

		updateHorizontalMovement();

		m_squash_frame_seconds -= m_delta_time;

		if (m_squash_frame_seconds <= 0)
		{
			bounceOffGround();
			setFrameSquash2Stretch();

			max_velocity.x = maxSpeedNormalX;

			// Set next mode.
			mode = Mode::AIR;
		}
	}


	void Player::bounceOffGround()
	{
		/* Restore true position and send ball upwards. If velocity.y is not at max,
			undo the velocity incease from previous acceleration (applied after movement,
			but not yet ~used~). Otherwise, at 100% elasticity, the ball will bounce
			higher than it started. */

		// Reverse velocity (bounce!).
		velocity.y = -(ground_bounce_power);
	}


	/*
		This section covers when the ball hits / clings to a wall and bounces off.
	*/


	void Player::updateWall()
	{
		/* If trigger window closes with no action, wall bounces weakly off wall.
			Otherwise it has more powerful bounce. */
				
		// Press down - soft release.
		if (m_downKey.isActive)
		{
			BounceStrength wall_bounce_x_power{ BounceStrength::WEAK };
			BounceStrength wall_bounce_y_power{ BounceStrength::NONE };

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
		// Press up - high vertical jump, less horizontal movement.
		else if (m_upKey.isActive)
		{
			BounceStrength wall_bounce_x_power{ BounceStrength::WEAK };
			BounceStrength wall_bounce_y_power{ BounceStrength::STRONG };

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
		// Press left/right (away from wall) - good horizontal movement, less vertical jump.
		else if(wallBounceTrigger == Trigger::LEFT && m_leftKey.isActive || wallBounceTrigger == Trigger::RIGHT && m_rightKey.isActive)
		{			
			BounceStrength wall_bounce_x_power{ BounceStrength::STRONG };
			BounceStrength wall_bounce_y_power{ BounceStrength::WEAK };

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
	}


	void Player::bounceOffWall(BounceStrength& wall_bounce_x_power, BounceStrength& wall_bounce_y_power)
	{
		/* Set velocity and if any direction lock, as well as next sprite. AIR mode
			handles movement and collision. */

		setEjectionSpeedY(wall_bounce_y_power);
		setEjectionSpeedX(wall_bounce_x_power);

		// Set sprite and mode.

		mode = Mode::AIR;
	}


	void Player::setEjectionSpeedY(BounceStrength& wall_bounce_y_power)
	{
		/* Wall bounce always has "up" vertical velocity. */

		float multiplier{ 0.0f };
		switch (wall_bounce_y_power)
		{
		case BounceStrength::NONE:
			multiplier = 0.0f;
			break;
		case BounceStrength::WEAK:
			multiplier = 0.3f;
			break;
		case BounceStrength::STRONG:
			multiplier = 1.1f;
			break;
		}

		velocity.y = -1 * ground_bounce_power * multiplier;
	}


	void Player::setEjectionSpeedX(BounceStrength& wall_bounce_x_power)
	{
		/* Horizontal velocity needs direction away from wall. Temporarily increase
			max horizontal velocity from a strong wall bounce. Will be reset on squash. */

		switch (wall_bounce_x_power)
		{
		case BounceStrength::NONE:
			max_velocity.x = 1.0f;
			break;
		case BounceStrength::WEAK:
			max_velocity.x = maxSpeedNormalX;
			break;
		case BounceStrength::STRONG:
			max_velocity.x = maxSpeedNormalX * 1.5f;
			break;
		}

		velocity.x = max_velocity.x * (wallBounceTrigger == Trigger::LEFT ? -1 : 1);
	}


	/*
		This section covers when the ball hits the ceiling.
	*/


	void Player::updateCeiling()
	{
		/* "Freeze" ball in squash mode for squash frame count. AIR mode 
			handles movement and collisions. */

		m_squash_frame_seconds -= m_delta_time;

		if (m_squash_frame_seconds <= 0)
		{
			setFrameSquash2Stretch();

			max_velocity.x = maxSpeedNormalX;

			// Set next mode.
			mode = Mode::AIR;
		}
	}


	/*
		This section deals with changing the sprite image / frame.
	*/


	void Player::updateFrameStretch2Normal()
	{
		/* Countdown until switching from stretched image to normal image. Must
			also not be direction locked from a weak wall bounce. */
		
		stretchFrameCount -= m_delta_time;

		if (stretchFrameCount <= 0)
		{
			m_sprite.SetFrame(0);
		}
	}


	void Player::setFrameNormal2Squash()
	{
		/* If velocity was fast enough to generate a frame of squash, change image. */

		if (m_squash_frame_seconds > 0.0f)
		{
			m_sprite.SetFrame(1);
		}
	}


	void Player::setFrameSquash2Stretch()
	{
		// Set mode to stretch.
		if (stretchFrameCount > 0.0f)
		{
			m_sprite.SetFrame(2);
		}
	}
};