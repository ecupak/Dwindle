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
	Player::Player(keyState& leftKey, keyState& rightKey, keyState& upKey, keyState& downKey) :
		m_sprite{ Sprite{new Surface("assets/ball.png"), 3, true} }, // player.png is 40px by 40px.
		half_height{ m_sprite.GetHeight() / 2 },
		half_width{ m_sprite.GetWidth() / 2 },
		half_size{ half_height },
		m_leftKey{ leftKey }, m_rightKey{ rightKey }, m_upKey{ upKey }, m_downKey{ downKey },
		m_player_echo{ m_sprite }
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

		// If dead, determine when free fall happens.
		if (state == State::DEAD && mode != Mode::FREE_FALL)
		{
			//m_dead_timer += deltaTime;

			// If at a full rest, or it has been 4 seconds since death.
			if ((m_is_horizontal_at_rest && m_is_vertical_at_rest) || m_dead_timer >= 4.0f)
			{
				m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_IN_FREE_FALL });
				mode = Mode::FREE_FALL;
			}
		}

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
		case Mode::FREE_FALL:
			updateFreeFall();
			break;
		case Mode::NONE:
		default:
			updateAir();
			break;
		}

		UpdatePosition();
	}


	void Player::Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom)
	{
		/*for (DetectorPoint& point : points)
		{
			viewable_layer->Box(point.left - c_left, point.top - c_top, point.right - c_left, point.bottom - c_top, 0xFFFFFFFF);
		}*/

		m_sprite.SetFrame(m_frame_id);
		m_sprite.Draw(viewable_layer, position.x - c_left, position.y - c_top);
		m_player_echo.Draw(viewable_layer, c_left, c_top);
	}


	int Player::GetStartingLife()
	{
		return m_player_strength;
	}


	// Prepare for start of level (current or new).
	void Player::RestoreDefaults()
	{
		// Restore defaults.
		m_dead_timer = 0.0f;
		m_free_fall_frame_count = 0;

		m_is_vertical_at_rest = false;
		m_is_horizontal_at_rest = false;
		m_is_echo_update_enabled = true;

		m_player_strength = m_player_max_strength;
		m_ground_bounce_power = m_max_ground_bounce_power;
		
		// Set life display to max strength/life.
		m_life_socket->SendMessage(LifeMessage{ m_player_strength, 1.0f });

		// Undo death.
		mode = Mode::AIR;
		state = State::ALIVE;
		for (DetectorPoint& point : points)
		{
			point.UpdateState(state);
		}
	}


	// Place at start of level (current or new). Coming out of falling transition.
	void Player::TransitionToPosition(vec2& new_position)
	{
		// Determine amount moved and apply to echoes.
		vec2 reset_delta{
				new_position.x - position.x,
				new_position.y - position.y
		};
		m_player_echo.ApplyDelta(reset_delta);

		// Keep same falling speed to maintain illusion that
		// player did not move during transition. If speed
		// is slower, echoes will "catch up" to player as they
		// fall to ground, instead of tracing the same path.
		velocity.y = max_velocity.y;

		// Set new position and bounds for player and detector points.
		SetPosition(new_position);
	}


	void Player::SetPosition(vec2& new_position)
	{
		position = new_position;

		SetCenterAndBounds();

		for (DetectorPoint& point : points)
		{
			point.SetPosition(center, half_size - 4);
		}

	}


	void Player::UpdatePosition()
	{		
		prev_position = position;

		if (m_is_echo_update_enabled)
		{
			m_player_echo.Update(position, m_frame_id);
		}		

		float half_t2{ 0.5f * m_delta_time * m_delta_time };

		// If player isn't pressing left/right and we have no velocity, don't move.
		if (m_allow_horizontal_movement)
		{
			distance.x = (direction.x == 0.0f && velocity.x == 0.0f) ? 0.0f : GetDistanceToMove(VectorIndex::X, half_t2);
		}
		else
		{
			distance.x = 0.0f;
		}

		distance.y = GetDistanceToMove(VectorIndex::Y, half_t2);

		position.x += distance.x;
		position.y += distance.y;
 
		SetCenterAndBounds();

		for (DetectorPoint& point : points)
		{
			point.UpdatePosition(velocity, distance);
		}
	}


	float Player::GetDistanceToMove(VectorIndex vector_index, float pre_calculated_half_t2)
	{
		int vec2_index{ 0 };
		switch (vector_index)
		{
		case VectorIndex::X:
			vec2_index = 0;
			break;
		case VectorIndex::Y:
			vec2_index = 1;
			break;
		}

		// distance = velocity * time + 1/2 * acceleration * time^2.
		return ((velocity[vec2_index] * m_delta_time) + (velocity[vec2_index] * pre_calculated_half_t2))
			// mutliplied so acceleration and velocity can stay as manageable numbers.
			* magnitude_coefficient[vec2_index];			
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

	
	void Player::RegisterGameSocket(Socket<GameMessage>* game_socket)
	{
		m_game_socket = game_socket;
	}

	void Player::RegisterGlowSocket(Socket<GlowMessage>* glow_socket)
	{
		m_glow_socket = glow_socket;
	}


	void Player::RegisterCameraSocket(Socket<CameraMessage>* camera_socket)
	{
		m_camera_socket = camera_socket;
	}


	void Player::RegisterLifeSocket(Socket<LifeMessage>* life_socket)
	{
		m_life_socket = life_socket;
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

				// Store the first instance of a ricochet collision - ignore the rest.
				// If there are multiple detected ricochets, there is little gain from
				// trying to determine which is the more correct one to use, which is 
				// a difficult process. There is no situation in which something bad will
				// happen by taking the first one and ignoring the rest.
				if (point.isRicochetCollision && !is_ricochet_set)
				{
					ricochet_velocity = point.GetNewVelocity();
					is_ricochet_set = true;
				}
			}
		}

		// If at least 1 axis is not 0, there was a collision that needs to be handled.
		if (delta_position.x != 0.0f || delta_position.y != 0.0f)
		{
			// Apply the delta change in position (pushed out of obstacle).
			position += delta_position;
			SetCenterAndBounds();
			for (DetectorPoint& point : points)
			{
				point.ApplyDeltaPosition(delta_position);
				point.ClearCollisions();
			}

			// Determine if safe glow orb will spawn.
			// - Spawns if 'full contact' made with surface.
			// - Spawns if safe glow orb does not already exist at location.
			// - Reduces life/opacity strength by 1 point.
			bool is_safe_glow_needed{ false };
			if (state == State::ALIVE)
			{
				// Determine if safe glow orb is needed.
				if (new_mode & ~NONE)
				{
					if (new_mode & GROUND)
					{
						is_safe_glow_needed = GetIsSafeGlowNeeded(BOTTOM);
					}
					else if (new_mode & WALL)
					{
						is_safe_glow_needed = GetIsSafeGlowNeeded(post_id);
					}
					else
					{
						is_safe_glow_needed = GetIsSafeGlowNeeded(TOP); // Ceiling hits should not make safe glow orbs.
					}
				}
				 
				// If safe glow orb needed, decrease life/opacity strength.
				if (is_safe_glow_needed)
				{
					if (--m_player_strength >= 0)
					{
						// Note that for the life display, it always respects the minimum brightness (m_player_min_brightness_buffer) set by user.					
						m_life_socket->SendMessage(LifeMessage{ m_player_strength, 1.0f * Max(m_player_strength, m_player_min_brightness_buffer) / m_player_max_strength });
					}
				}

				// If player is now dead, update state.
				if (m_player_strength < 0)
				{
					state = State::DEAD;

					// Ricochet velocitis will decrease while in dead bounce.
					for (DetectorPoint& point : points)
					{
						point.UpdateState(state);
					}

					// Do not place a safe glow orb at location.
					is_safe_glow_needed = false;

					// Tell game that player is dead.
					m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_DEATH });
				}
			}
			// During death bounces, decrease brightness buffer so glow orbs approach 0 opacity.
			else // if (state == State::DEAD)
			{
				m_player_brightness_buffer -= m_player_min_brightness_buffer * 0.35f;
				m_ground_bounce_power = Max(0.0f, m_ground_bounce_power - 1.0f);
			}

			// While free-falling, player is in transition between level (new or reset). 
			// - Do not create glow orbs.
			// - Do not change modes (will be reset to AIR by game).
			if (mode != Mode::FREE_FALL)
			{
				/*
					Create glow orbs and set player's mode based on collision.
				*/
				float calculated_opacity{ Max(1.0f * m_player_strength, m_player_brightness_buffer) / m_player_max_strength };

				// Update glow socket.
				if (new_mode & ~NONE)
				{
					m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, center, calculated_opacity, CollidableType::FULL_GLOW, is_safe_glow_needed });
				}
				else if (is_ricochet_set)
				{
					m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, center, calculated_opacity, CollidableType::TEMP_GLOW });
				}

				// Set next mode for player.
				if (new_mode & ~NONE)
				{
					if (new_mode & GROUND)
					{
						handleGroundCollision();
					}
					else if (new_mode & WALL)
					{
						handleWallCollision(post_id);
					}
					else
					{
						handleCeilingCollision();
					}
				}
				// Ricochet if a corner was hit. Ricochet even during death bounces.
				else if (is_ricochet_set)
				{
					velocity = ricochet_velocity;
				}
			}
		}
		// If no collisions happened, remove collisions (overlaps with no line intersects) stored in points.
		else // if (delta_position.x == 0.0f && delta_position.y == 0.0f)
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
		return (T(0) < val) - (val < T(0)); // returns one of: -1, 0, 1
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
	}


	void Player::updateVerticalMovement()
	{
		/* Check if sprite image can be changed to normal. Stretched image is
			immediate result of squashed/GROUND state. Update vertical movement
			and velocity.y, keeping within bounds. */

		updateFrameStretch2Normal();

		// V.final = V.initial + (acceleration * time);
		velocity.y += acceleration.y * m_delta_time;
		velocity.y = Min(velocity.y, max_velocity.y); // we only care about velocity.y getting too positive.
	}


	void Player::updateHorizontalMovement()
	{
		/* Update horizontal position, unless direction locked from weak wall bounce. */

		// Only take input while player is alive.
		if (state == State::ALIVE)
		{
			direction.x = -(m_leftKey.isActive) + m_rightKey.isActive;
		}
		else
		{
			direction.x = 0.0f;
		}

		// If there is input, move.
		if (direction.x != 0.0f)
		{			
			// Only move if in the air (don't move while clinging to walls or squashed on ground).
			if (mode == Mode::AIR)
			{
				velocity.x += direction.x * acceleration.x * m_delta_time;
				velocity.x = Clamp(velocity.x, -max_velocity.x, max_velocity.x);
			}
		}
		else // if (direction.x == 0.0f)
		{
			// Set direction to be the opposite of current velocity.x (we are reducing velocity).
			direction.x = GetSign(velocity.x) * -1;

			if (state == State::ALIVE)
			{
				velocity.x += direction.x * acceleration.x * m_delta_time;

				if (fabsf(velocity.x) < m_horizontal_dead_zone)
				{
					velocity.x = 0.0f;
				}
			}
			else // if (state == State::DEAD)
			{
				// Slower loss of acceleration. Makes it more fun to watch.
				velocity.x += direction.x * m_acceleration_x_dead_dampening * acceleration.x * m_delta_time;

				// Dead bouncing doens't lose velocity.x until velocity.y has stopped (no bounce, only roll).
				if (m_is_vertical_at_rest && fabsf(velocity.x) < m_horizontal_dead_zone)
				{
					velocity.x = 0.0f;
					m_is_horizontal_at_rest = true;
				}
			}

			/*
			if (m_is_vertical_at_rest && fabsf(velocity.x) < m_horizontal_dead_zone)
			{
				velocity.x = 0.0f;
				m_is_horizontal_at_rest = true;
			}
			*/
		}
	}


	void Player::handleGroundCollision()
	{
		/* Cheat the y position so the ball can appear to be above the ground.
			Determine if ball will be squashed or come to a complete rest. */

		
		{
			prepareForGroundMode();
		}

		// Update camera focus.
		m_camera_socket->SendMessage(CameraMessage{ center, Location::GROUND });

		// Set next mode.
		mode = Mode::GROUND;
	}


	


	void Player::prepareForGroundMode()
	{
		/* Remove any directional lock since player can't move ball while it is
			squashed anyway. Calculate the squash and stretch frame counts. Slower
			speeds result in less frames of each. GROUND mode squashes the ball. */

		velocity.y = 0.0f;

		if (state == State::ALIVE)
		{
			m_allow_horizontal_movement = false;

			setSquashFrameCount();
			setStretchFrameCount();
			setFrameNormal2Squash();
		}

		/*
			Update positions and them above ground after collision.
			Mode::GROUND does not update positions while in delay.
		*/
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

		m_squash_frame_seconds = 0.24f; //static_cast<int>(squashValue);		
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

		// Cling to wall if alive.
		if (state == State::ALIVE)
		{
			velocity = vec2{ 0.0f,0.0f };

			bool isOnLeftWall = (post_id == LEFT);

			if (isOnLeftWall)
				prepareForWallMode(Trigger::RIGHT);
			else
				prepareForWallMode(Trigger::LEFT);

			// Update camera focus.
			m_camera_socket->SendMessage(CameraMessage{ center, Location::WALL });

			// Set next mode.
			mode = Mode::WALL;
		}
		// Bounce off wall if dead.
		else
		{
			velocity.x *= -1;
		}
	}


	void Player::prepareForWallMode(Trigger trigger)
	{
		/* Reposition ball just off of wall and lose all velocity. Register trigger and
			set trigger duration. Update the sprite. WALL mode sticks ball on wall. */

		m_leftKey.isActive = false;
		m_rightKey.isActive = false;
		m_upKey.isActive = false;
		m_downKey.isActive = false;

		wallBounceTrigger = trigger;
		//triggerFrameCount = 1.0f;
		
		// Need to figure out how to rotate sprite for side squash. Matrix rotation preferred.
		// But can also create static sprite frames for each wall and ceiling.
		//m_sprite.SetFrame(newFrame);
	}


	void Player::handleCeilingCollision()
	{
		/* Based on wall, prepare for WALL mode. Frame int is based on 3 images per
			orientation and style of ball. */

		prepareForCeilingMode();

		// Update camera focus.
		m_camera_socket->SendMessage(CameraMessage{ center, Location::OTHER });

		// Set next mode.
		mode = Mode::CEILING;
	}


	void Player::prepareForCeilingMode()
	{
		/* Remove any directional lock since player can't move ball while it is
			squashed anyway. Calculate the squash and stretch frame counts. Slower
			speeds result in less frames of each. GROUND mode squashes the ball. */

		if (state == State::ALIVE)
		{
			velocity.y = 0.0f;

			// Need to figure out how to rotate sprite for side squash. Matrix rotation preferred.
			// But can also create static sprite frames for each wall and ceiling.
			
			/*setSquashFrameCount();
			setStretchFrameCount();
			setFrameNormal2Squash();*/
		}
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
			// If bouncing with not enough force, come to a stop.
			if (state == State::DEAD && m_ground_bounce_power <= m_ground_bounce_power_dead_zone)
			{
				stopBouncing();
			}
			// Otherwise bounce off ground (even while dead).
			else
			{
				bounceOffGround();
				setFrameSquash2Stretch();

				m_allow_horizontal_movement = true;
				max_velocity.x = maxSpeedNormalX;

				// Set next mode.
				mode = Mode::AIR;
			}
		}
	}

	void Player::stopBouncing()
	{
		/* Lose all velocity and set to normal image. REST mode is a dead end. */
		velocity.y = 0.0f;
		m_frame_id = 0;

		m_is_vertical_at_rest = true;
	}


	void Player::bounceOffGround()
	{
		/* Restore true position and send ball upwards. If velocity.y is not at max,
			undo the velocity incease from previous acceleration (applied after movement,
			but not yet ~used~). Otherwise, at 100% elasticity, the ball will bounce
			higher than it started. */

		// Reverse velocity (bounce!).
		velocity.y = -(m_ground_bounce_power);
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

		velocity.y = -1 * m_ground_bounce_power * multiplier;
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


	void Player::updateFreeFall()
	{
		if (velocity.y == max_velocity.y)
		{
			++m_free_fall_frame_count;
			if (m_free_fall_frame_count >= 30)
			{
				velocity.y = 0.0f;
				m_is_echo_update_enabled = false;
				m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_SUSPENDED });
			}
		}
		else if (m_free_fall_frame_count < 30)
		{
			updateVerticalMovement();
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
			m_frame_id = 0;
		}
	}


	void Player::setFrameNormal2Squash()
	{
		/* If velocity was fast enough to generate a frame of squash, change image. */

		if (state == State::ALIVE)
		{
			if (m_squash_frame_seconds > 0.0f)
			{
				m_frame_id = 1;
			}
		}
	}


	void Player::setFrameSquash2Stretch()
	{
		if (state == State::ALIVE)
		{
			// Set mode to stretch.
			if (stretchFrameCount > 0.0f)
			{
				m_frame_id = 2;
			}
		}
	}
};