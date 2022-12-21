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
		m_sprite{ Sprite{new Surface("assets/player.png"), 6, true} },
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


	void Player::Draw(Surface* screen)
	{
		for (DetectorPoint& point : points)
		{
			screen->Box(point.left, point.top, point.right, point.bottom, 0xFFFFFFFF);
		}

		//m_sprite.Draw(screen, position.x, position.y);
	}


	void Player::SetPosition(vec2& start_position)
	{
		position = start_position;
		SetCenter();
		
		for (DetectorPoint& point : points)
		{
			point.SetPosition(center, half_size);
		}
	}


	void Player::UpdatePosition()
	{
		prev_position = position;
		position += speed;
		SetCenter();

		for (DetectorPoint& point : points)
		{
			point.UpdatePosition(speed);
		}
	}


	void Player::SetCenter()
	{
		center = vec2(position.x + half_width, position.y + half_height);
	}


	std::vector<DetectorPoint>& Player::GetCollisionPoints()
	{	
		return points;
	}

	
	void Player::RegisterGlowSocket(GlowSocket& glow_socket)
	{
		m_glow_socket = &glow_socket;
	}


	void Player::ResolveCollision(Collidable*& collision)
	{
		collisions.push_back(collision);
	}


	void Player::ProcessCollisions()
	{
		int new_mode{ NONE };
		bool is_ricochet_set{ false };
		std::vector<int> posts;		
		vec2 delta_position{ 0.0f, 0.0f };
		vec2 ricochet_speed{ 0.0f, 0.0f };

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
					posts.push_back(point.post);
				
				// If a ricochet collision, store the first instance.
				if (point.isRicochetCollision && !is_ricochet_set)
				{
					ricochet_speed = point.speed;
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
			SetCenter();
			for (DetectorPoint& point : points)
			{
				point.ApplyDeltaPosition(delta_position);
			}


			// Update mode. If no mode, check if ricochet and apply new speed.
			if (new_mode & ~NONE)
			{
				if (new_mode & GROUND)
					handleGroundCollision();
				else if (new_mode & WALL)
					handleWallCollision(posts);
				else
					handleCeilingCollision();
			}
			else if (is_ricochet_set)
				speed = ricochet_speed;


			// Update GlowSocket.
			if (new_mode & ~(NONE))
			{
				printf("Player center: %f, %f\n", center.x, center.y);
				m_glow_socket->SendMessage(center, false);
			}
			else if (is_ricochet_set)
				m_glow_socket->SendMessage(center, true);
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
			and speed.y, keeping within bounds. */

		//updateFrameStretch2Normal();

		speed.y += acceleration.y * m_delta_time;
	}


	void Player::updateHorizontalMovement()
	{
		/* Update horizontal position, unless direction locked from weak wall bounce. */

		direction.x = -(m_leftKey.isActive) + m_rightKey.isActive;
		if (direction.x != 0)
		{
			if (mode == Mode::AIR)
			{
				speed.x += direction.x * acceleration.x * m_delta_time;
				speed.x = Clamp(speed.x, -maxSpeed.x, maxSpeed.x);
			}
		}
		else
		{
			if (speed.x != 0.0f)
			{
				if (fabsf(speed.x) < 0.3f) //dead_zone.x;
				{
					speed.x = 0.0f;
				}
				else if (mode == Mode::AIR)
				{
					float speed_x = fabsf(speed.x) - (m_delta_time * 2.5f);
					speed_x = Max(speed_x, 0.0f);
					speed.x = speed_x * (speed.x > 0.0f ? 1 : -1);
				}
				else if (mode == Mode::GROUND)
				{
					speed.x = 0.0f;
				}
			}
		}
	}


	void Player::handleGroundCollision()
	{
		/* Cheat the y position so the ball can appear to be above the ground.
			Determine if ball will be squashed or come to a complete rest. */

		// Set next mode.
		if (deadZone > fabsf(speed.y))
		{
			stopBouncing();
		}
		else
		{
			prepareForGroundMode();
		}		
	}


	void Player::stopBouncing()
	{
		/* Lose all vertical speed and set to normal image. REST mode is a dead end. */

		speed.y = 0.0f;
		m_sprite.SetFrame(0);
		mode = Mode::REST;
	}


	void Player::prepareForGroundMode()
	{
		/* Remove any directional lock since player can't move ball while it is
			squashed anyway. Calculate the squash and stretch frame counts. Slower
			speeds result in less frames of each. GROUND mode squashes the ball. */

		//UpdatePosition(0.0f, 0.0f);

		//directionLockedFrameCount = 0;

		setSquashFrameCount();
		setStretchFrameCount();

		setFrameNormal2Squash();

		// Update positions.
		for (DetectorPoint& point : points)
		{
			point.UpdatePreviousPosition();
		}

		// Set next mode.
		mode = Mode::GROUND;
	}


	void Player::setSquashFrameCount()
	{
		/* Squash delay / animation is a function of impact speed. All in an effort
			to reduce / remove squashing when at low speeds. Around 4 speed there
			should be no squashing at all, but kept it generalized just in case. */

			// Easier to step through.
		float squashValue = fabsf(speed.y);
		squashValue -= squashDampeningMagnitude;
		squashValue *= squashDampeningCoefficient;
		squashValue = ceil(squashValue) - 1;

		squashFrameCount = 0.5f; //static_cast<int>(squashValue);
	}


	void Player::setStretchFrameCount()
	{
		/* Stretch animation is a function of squash delay. Must be calculated
			before the squash frames begin counting down. */

		stretchFrameCount = (squashFrameCount - 1) * 2;
	}


	void Player::handleWallCollision(std::vector<int>& posts)
	{
		/* Based on wall, prepare for WALL mode. Frame int is based on 3 images per
			orientation and style of ball. */

		bool isOnLeftWall = (std::find(posts.begin(), posts.end(), LEFT) != posts.end());

		if (isOnLeftWall)
			prepareForWallMode(Trigger::RIGHT);
		else
			prepareForWallMode(Trigger::LEFT);

		m_leftKey.isActive = false;
		m_rightKey.isActive = false;
		m_upKey.isActive = false;
		m_downKey.isActive = false;
	}


	void Player::prepareForWallMode(Trigger trigger)
	{
		/* Reposition ball just off of wall and lose all speed. Register trigger and
			set trigger duration. Update the sprite. WALL mode sticks ball on wall. */

		speed.x = 0.0f;

		//UpdatePosition(0.0f, 0.0f);

		wallBounceTrigger = trigger;
		triggerFrameCount = 1.0f;
		
		//m_sprite.SetFrame(newFrame);

		// Set next mode.
		mode = Mode::WALL;
	}


	void Player::handleCeilingCollision()
	{
		/* Based on wall, prepare for WALL mode. Frame int is based on 3 images per
			orientation and style of ball. */

		prepareForCeilingMode();
	}


	void Player::prepareForCeilingMode()
	{
		/* Remove any directional lock since player can't move ball while it is
			squashed anyway. Calculate the squash and stretch frame counts. Slower
			speeds result in less frames of each. GROUND mode squashes the ball. */

		//UpdatePosition(0.0f, 0.0f);

		speed.y = 0.0f;

		//directionLockedFrameCount = 0;

		setSquashFrameCount();
		setStretchFrameCount();

		setFrameNormal2Squash();

		// Set next mode.
		mode = Mode::CEILING;
	}


	/*
		This section covers when the ball hits the ground and bounces back up.
	*/


	void Player::updateGround()
	{
		/* "Freeze" ball in squash mode for squash frame count. Then bounce back up
			with equal speed to landing speed. Reset maxSpeed.x to normal speed (is
			increased after a strong wall bounce). AIR mode handles movement and
			collisions. */

		updateHorizontalMovement();

		squashFrameCount -= m_delta_time;

		if (squashFrameCount <= 0)
		{
			bounceOffGround();
			setFrameSquash2Stretch();

			maxSpeed.x = maxSpeedNormalX;

			// Set next mode.
			mode = Mode::AIR;
		}
	}


	void Player::bounceOffGround()
	{
		/* Restore true position and send ball upwards. If speed.y is not at max,
			undo the speed incease from previous acceleration (applied after movement,
			but not yet ~used~). Otherwise, at 100% elasticity, the ball will bounce
			higher than it started. */

		// Reverse speed (bounce!).
		speed.y = -(ground_bounce_power);
	}


	/*
		This section covers when the ball hits / clings to a wall and bounces off.
	*/


	void Player::updateWall()
	{
		/* If trigger window closes with no action, wall bounces weakly off wall.
			Otherwise it has more powerful bounce. */

		// Press into the wall - drop down.
		if (wallBounceTrigger == Trigger::LEFT && m_rightKey.isActive
			|| wallBounceTrigger == Trigger::RIGHT && m_leftKey.isActive)
		{
			BounceStrength wall_bounce_x_power = BounceStrength::NONE;
			BounceStrength wall_bounce_y_power = BounceStrength::NONE;
			/*m_leftKey.isActive = false;
			m_rightKey.isActive = false;*/

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
		// Press down - soft release.
		else if (m_downKey.isActive)
		{
			BounceStrength wall_bounce_x_power = BounceStrength::WEAK;
			BounceStrength wall_bounce_y_power = BounceStrength::NONE;
			/*m_downKey.isActive = false;*/

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
		// Press up or away - strong release.
		else if (m_upKey.isActive
			|| wallBounceTrigger == Trigger::LEFT && m_leftKey.isActive
			|| wallBounceTrigger == Trigger::RIGHT && m_rightKey.isActive)
		{			
			BounceStrength wall_bounce_x_power = (m_upKey.isActive ? BounceStrength::WEAK : BounceStrength::STRONG);
			BounceStrength wall_bounce_y_power = (m_upKey.isActive ? BounceStrength::STRONG : BounceStrength::WEAK);
			/*m_upKey.isActive = false;
			m_leftKey.isActive = false;
			m_rightKey.isActive = false;*/

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
	}


	void Player::bounceOffWall(BounceStrength& wall_bounce_x_power, BounceStrength& wall_bounce_y_power)
	{
		/* Set speed and if any direction lock, as well as next sprite. AIR mode
			handles movement and collision. */

		setEjectionSpeedY(wall_bounce_y_power);
		setEjectionSpeedX(wall_bounce_x_power);

		// Set sprite and mode.

		mode = Mode::AIR;
	}


	void Player::setEjectionSpeedY(BounceStrength& wall_bounce_y_power)
	{
		/* Wall bounce always has "up" vertical speed. */

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
			multiplier = 1.3f;
			break;
		}

		speed.y = -1 * ground_bounce_power * multiplier;
	}


	void Player::setEjectionSpeedX(BounceStrength& wall_bounce_x_power)
	{
		/* Horizontal speed needs direction away from wall. Temporarily increase
			max horizontal speed from a strong wall bounce. Will be reset on squash. */

		switch (wall_bounce_x_power)
		{
		case BounceStrength::NONE:
			maxSpeed.x = 1.0f;
			break;
		case BounceStrength::WEAK:
			maxSpeed.x = maxSpeedNormalX;
			break;
		case BounceStrength::STRONG:
			maxSpeed.x = maxSpeedNormalX * 2;
			break;
		}

		speed.x = maxSpeed.x * (wallBounceTrigger == Trigger::LEFT ? -1 : 1);
	}


	/*
		This section covers when the ball hits the ceiling.
	*/


	void Player::updateCeiling()
	{
		/* "Freeze" ball in squash mode for squash frame count. AIR mode 
			handles movement and collisions. */

		squashFrameCount -= m_delta_time;

		if (squashFrameCount <= 0)
		{
			setFrameSquash2Stretch();

			maxSpeed.x = maxSpeedNormalX;

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

		//if (stretchFrameCount <= 0 && directionLockedFrameCount <= 0)
		if (stretchFrameCount <= 0)
		{
			m_sprite.SetFrame(0);
		}
		else
		{
			stretchFrameCount--;
		}
	}


	void Player::updateFrameDirectionLockRelease()
	{
		/* Switch to normal image once direction lock has ended. Used to signal to
			player when control is restored. Must also not be in stretch mode from
			a ground bounce (or else stretch image is ended too early). */

		//if (directionLockedFrameCount <= 0 && stretchFrameCount <= 0)
		if (stretchFrameCount <= 0)
		{
			m_sprite.SetFrame(0);
		}
	}


	void Player::setFrameNormal2Squash()
	{
		/* If speed was fast enough to generate a frame of squash, change image. */

		if (squashFrameCount > 0)
		{
			m_sprite.SetFrame(1);
		}
	}


	void Player::setFrameSquash2Stretch()
	{
		// Set mode to stretch.
		if (stretchFrameCount > 0)
		{
			m_sprite.SetFrame(2);
		}
	}


	void Player::setFrameAfterWallBounce(bool isWeakBounce)
	{
		/* Weak bounce sprite is unfilled until control is given back to player. */

		m_sprite.SetFrame(isWeakBounce ? 3 : 0);
	}
};