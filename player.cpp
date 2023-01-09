#pragma once

#include "player.h"

#include <algorithm>
#include <map>


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
	constexpr int OBSTACLE{ 1 };
	constexpr int SMOOTH_CORNER{ 2 };
	constexpr int ROUGH{ 4 };
	constexpr int ROUGH_CORNER{ 8 };


	// Constructor.
	Player::Player(keyboard_manager2& keyboard_manager) :
		Collidable{ CollidableInfo{ CollidableType::PLAYER, CollisionLayer::CAMERA, CollisionMask::NONE, 10 } },
		m_sprite{ Sprite{new Surface("assets/ball.png"), 3, true} },
		m_keyboard_manager{ keyboard_manager },
		m_player_echo{ m_sprite }
	{
		// Create array of points that will go around circle.
		for (int i{ 0 }; i < POINTS; i++)
		{
			points.push_back(DetectorPoint{ i });
		}				
	}


	void Player::Update(float deltaTime)
	{
		m_delta_time = deltaTime;


		// If dead, determine when free fall happens.
		//if (state == State::DEAD && mode != Mode::FREE_FALL && mode != Mode::SUSPENDED)
		if (m_is_dead_timer_enabled)
		{
			m_dead_timer += deltaTime;

			// If at a full rest, or it has been 4 seconds since death.
			if (m_dead_timer >= m_dead_time_limit)
			{
				m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_IN_FREE_FALL });
				mode = Mode::FREE_FALL;
				m_is_dead_timer_enabled = false;
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
		case Mode::SUSPENDED:
			break;
		case Mode::FREE_FALL:
			updateFreeFall();
			break;
		case Mode::GAMEOVER:
			updateGameOver();
			break;
		case Mode::NONE:
		default:
			updateAir();
			break;
		}

		UpdatePosition();
	}


	void Player::Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity)
	{
		if (m_is_debug_mode_on)
		{
			for (DetectorPoint& point : points)
			{
				viewable_layer->Box(point.left - c_left, point.top - c_top, point.right - c_left, point.bottom - c_top, 0xFFFFFFFF);
			}
		}
		else
		{
			// Draw over that with echoes.
			m_player_echo.Draw(viewable_layer, c_left, c_top);

			// Finally draw player on top of all.
			m_sprite.SetFrame(m_frame_id);
			m_sprite.Draw(viewable_layer, position.x - c_left, position.y - c_top);
		}		
	}


	int Player::GetStartingLife()
	{
		return m_player_strength;
	}


	void Player::ToggleDebugMode()
	{
		m_is_debug_mode_on = !m_is_debug_mode_on;
	}

	
	void Player::SetMode(Mode new_mode)
	{
		mode = new_mode;
	}


	// Set mode for title screen use.
	void Player::SetTitleScreenMode()
	{
		SetGameScreenMode();
		mode = Mode::SUSPENDED;
		state = State::DEAD;

		// Adjust position so center is at given coordinates.
		/*SetPosition(vec2{
			position.x - (m_sprite.GetWidth() / 2),
			position.y - (m_sprite.GetHeight() / 2)
		});*/
	}

	
	void Player::SetStartingLife(int starting_life)
	{
		m_player_max_strength = starting_life;
	}


	// Prepare for start of level (current or new).
	void Player::SetGameScreenMode()
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
		m_life_socket->SendMessage(LifeMessage{ LifeAction::UPDATE, m_player_strength, 1.0f });

		// Undo death.
		mode = Mode::AIR;
		state = State::ALIVE;
		for (DetectorPoint& point : points)
		{
			point.UpdateState(state);
			point.m_is_at_finish_line = false;
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


	void Player::KeepFalling(bool is_set_to_keep_falling)
	{
		m_is_set_to_keep_falling = is_set_to_keep_falling;
	}


	void Player::SetPosition(vec2& new_position)
	{
		position = new_position;

		SetCenterAndBounds();

		for (DetectorPoint& point : points)
		{
			point.SetPosition(m_center, (m_sprite.GetWidth() * 0.5f) - 4);
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

		// Move same distance as moving object we are riding.
		if (m_tethered_object != nullptr)
		{
			distance += m_tethered_object->m_delta_position;

			if (m_is_untethering)
			{
				//distance += m_tethered_object->m_delta_position;
				m_ignore_tether_collisions = true;
				m_tethered_object = nullptr;
				
			}
		}

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
		int vec2_index{ vector_index == VectorIndex::X ? 0 : 1};

		// distance = velocity * time + 1/2 * acceleration * time^2.
		return ((velocity[vec2_index] * m_delta_time) + (velocity[vec2_index] * pre_calculated_half_t2))
			// mutliplied so acceleration and velocity can stay as manageable numbers.
			* magnitude_coefficient[vec2_index];			
	}


	void Player::SetCenterAndBounds()
	{
		m_center = vec2(
			position.x + (m_sprite.GetWidth() * 0.5f),
			position.y + (m_sprite.GetHeight() * 0.5f)
		);
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
	
	
	void Player::RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket)
	{
		m_collision_socket = collision_socket;
	}


	void Player::RegisterWithCollisionManager()
	{	
		std::vector<Collidable*> m_point_collidables;
		for (Collidable& collidable : points)
		{
			m_point_collidables.push_back(&collidable);
		}

		m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_PLAYER_POINT_LIST, m_point_collidables });
		m_collision_socket->SendMessage(CollisionMessage{ CollisionAction::UPDATE_UNIQUE_LIST, this });
	}


	void Player::ResolveCollisions()
	{
		if (IsCollisionHandlingDisabled()) return;		

		int new_mode{ NONE };
		bool is_ricochet_set{ false };
		int post_id{ -1 };
		m_delta_position = vec2{ 0.0f, 0.0f };
		vec2 ricochet_velocity{ 0.0f, 0.0f };		
		std::map<int, int> contact_points;

		// Get change in position after collision, and the new mode.
		for (DetectorPoint& point : points)		
		{	
			// Check for finish line collision.
			if (state == State::ALIVE && point.m_is_at_finish_line)
			{
				SetDeadState();				

				// Update all points to know finish line was crossed.
				// Otherwise, trying to cross while dead will ricochet.
				for (DetectorPoint& point : points)
				{
					point.m_is_at_finish_line = true;
				}

				// Allow velocity.x to start decreasing immediately. Faster ending then when life reaches 0.
				m_is_vertical_at_rest = true;

				// Tell game that level is completed.
				m_game_socket->SendMessage(GameMessage{ GameAction::LEVEL_COMPLETE });
			}

			// Gain life and update life hud.
			if (point.m_is_on_pickup)
			{
				m_player_strength += m_life_restored_by_pickup;
				m_life_socket->SendMessage(LifeMessage{ LifeAction::UPDATE, m_player_strength, 1.0f * Max(m_player_strength, m_player_min_brightness_buffer) / m_player_max_strength });
			}

			// Check for surface collisions.
			if (point.CheckForCollisions())
			{
				/*
					Update delta position (shifting out of collision object).
					Only keep the largest changes to x and y.
				*/
				vec2 point_delta_position{ point.GetDeltaPosition() };				
				m_delta_position.x = GetAbsoluteMax(point_delta_position.x, m_delta_position.x);
				m_delta_position.y = GetAbsoluteMax(point_delta_position.y, m_delta_position.y);

				/*
					Get new mode (hit a flat surface). If new mode is WALL,
					store the post info so we know which side of player the wall is.
				*/

				int detected_mode = point.GetNewMode();
				if (detected_mode != NONE)
				{
					new_mode |= detected_mode;
					contact_points.insert(std::pair<int, int>(detected_mode, point.post_id));
				}
								

				// Store the first instance of a ricochet collision - ignore the rest.
				// If there are multiple detected ricochets, there is little gain from
				// trying to determine which is the more correct one to use, which is 
				// a difficult process. There is no situation in which something bad will
				// happen by taking the first one and ignoring the rest.
				if (point.m_is_ricochet_collisions && !is_ricochet_set)
				{
					ricochet_velocity = point.GetNewVelocity();
					is_ricochet_set = true;
				}
			}
		}

		// If at least 1 axis is not 0, there was a collision that needs to be handled.
		if (m_delta_position.x != 0.0f || m_delta_position.y != 0.0f)
		{
			// Apply the delta change in position (pushed out of obstacle).
			position += m_delta_position;
			SetCenterAndBounds();
			for (DetectorPoint& point : points)
			{
				point.ApplyDeltaPosition(m_delta_position);				
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
						post_id = contact_points.at(GROUND);
						SetTether(post_id);
						is_safe_glow_needed = GetIsSafeGlowNeeded(BOTTOM);
					}
					else if (new_mode & WALL)
					{
						post_id = contact_points.at(WALL);
						SetTether(post_id);
						is_safe_glow_needed = GetIsSafeGlowNeeded(post_id);
					}					
				}
				 
				// If safe glow orb needed, decrease life/opacity strength.
				if (is_safe_glow_needed)
				{
					if (--m_player_strength >= 0)
					{
						// Note that for the life display, it always respects the minimum brightness (m_player_min_brightness_buffer) set by user.					
						m_life_socket->SendMessage(LifeMessage{LifeAction::UPDATE, m_player_strength, 1.0f * Max(m_player_strength, m_player_min_brightness_buffer) / m_player_max_strength });
					}

					if (m_is_tutorial_mode && m_player_strength < 0)
					{
						m_player_strength = m_player_max_strength;
						m_life_socket->SendMessage(LifeMessage{ LifeAction::TUTORIAL_SAVE, m_player_strength, 1.0f * Max(m_player_strength, m_player_min_brightness_buffer) / m_player_max_strength });
					}
				}

				// If player is now dead, update state.
				if (m_player_strength < 0)
				{
					SetDeadState();

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
					bool is_on_dangerous_obstacle{ GetIsOnDangerousObstacle(post_id) };
					m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, m_center, calculated_opacity, CollidableType::GLOW_ORB_FULL, SafeGlowInfo(is_safe_glow_needed, is_on_dangerous_obstacle, m_tethered_object) });
				}
				else if (is_ricochet_set)
				{
					m_glow_socket->SendMessage(GlowMessage{ GlowAction::MAKE_ORB, m_center, calculated_opacity, CollidableType::GLOW_ORB_TEMP });
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
		
		// Remove collisions (overlaps with no line intersects) stored in points.
		for (DetectorPoint& point : points)
		{
			point.ClearCollisions();
		}		
	}


	bool Player::IsCollisionHandlingDisabled()
	{
		if (m_is_collision_state_disabled || m_ignore_tether_collisions)
		{
			for (DetectorPoint& point : points)
			{
				point.ClearCollisions();
			}

			m_ignore_tether_collisions = false;

			return true;
		}
		return false;
	}


	void Player::SetDeadState()
	{
		state = State::DEAD;
		m_player_strength = 0;
		m_tethered_object = nullptr;
		m_is_dead_timer_enabled = true;

		// Ricochet velocitis will decrease while in dead bounce.
		for (DetectorPoint& point : points)
		{
			point.UpdateState(state);
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


	void Player::SetTether(int post_id)
	{
		if (points[post_id].m_is_tethered)
		{
			m_tethered_object = points[post_id].m_tethered_object;
			m_is_untethering = false;
		}
	}


	bool Player::GetIsSafeGlowNeeded(int post_id)
	{
		return points[post_id].m_is_safe_glow_needed;
	}


	bool Player::GetIsOnDangerousObstacle(int post_id)
	{
		return post_id > -1 && points[post_id].m_is_on_dangerous_obstacle;
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

		// updateFrameStretch2Normal();

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
			direction.x = -(m_keyboard_manager.IsPressed(SDL_SCANCODE_LEFT)) + (m_keyboard_manager.IsPressed(SDL_SCANCODE_RIGHT));
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
				// Slower loss of velocity. Makes it more fun to watch.
				// But if still moving after a few seconds post-death, pump the brakes.
				velocity.x += direction.x * acceleration.x * m_delta_time * (m_dead_timer < m_dead_time_limit ? m_acceleration_x_dead_dampening : 1);

				// Dead bouncing doesn't lose velocity.x until velocity.y has stopped (then becomes no bounce, only roll).
				if (m_is_vertical_at_rest && fabsf(velocity.x) < m_horizontal_dead_zone)
				{
					velocity.x = 0.0f;
					m_is_horizontal_at_rest = true;
				}
			}
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
		m_camera_socket->SendMessage(CameraMessage{ m_center, Location::GROUND });

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
			m_camera_socket->SendMessage(CameraMessage{ m_center, Location::WALL });

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

		//m_key_manager.GetKey(SDLK_LEFT).m_is_active = false;
		//m_key_manager.GetKey(SDLK_RIGHT).m_is_active = false;
		//m_key_manager.GetKey(SDLK_UP).m_is_active = false;
		//m_key_manager.GetKey(SDLK_DOWN).m_is_active = false;

		wallBounceTrigger = trigger;
		
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
		//m_camera_socket->SendMessage(CameraMessage{ center, Location::OTHER });

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
				m_is_untethering = true;

				bounceOffGround();
				setFrameSquash2Normal();

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
		if (m_keyboard_manager.IsJustPressed(SDL_SCANCODE_DOWN))
		{
			BounceStrength wall_bounce_x_power{ BounceStrength::WEAK };
			BounceStrength wall_bounce_y_power{ BounceStrength::NONE };

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
		// Press up - high vertical jump, less horizontal movement.
		else if (m_keyboard_manager.IsJustPressed(SDL_SCANCODE_UP))
		{
			BounceStrength wall_bounce_x_power{ BounceStrength::WEAK };
			BounceStrength wall_bounce_y_power{ BounceStrength::STRONG };

			bounceOffWall(wall_bounce_x_power, wall_bounce_y_power);
		}
		// Press left/right (away from wall) - good horizontal movement, less vertical jump.
		else if(wallBounceTrigger == Trigger::LEFT && m_keyboard_manager.IsJustPressed(SDL_SCANCODE_LEFT)
			|| wallBounceTrigger == Trigger::RIGHT && m_keyboard_manager.IsJustPressed(SDL_SCANCODE_RIGHT))
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

		m_is_untethering = true;

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


	void Player::updateCeiling()
	{
		/* "Freeze" ball in squash mode for squash frame count. AIR mode 
			handles movement and collisions. */

		m_squash_frame_seconds -= m_delta_time;

		if (m_squash_frame_seconds <= 0)
		{
			//setFrameSquash2Stretch();

			max_velocity.x = maxSpeedNormalX;

			// Set next mode.
			mode = Mode::AIR;
		}
	}


	void Player::updateFreeFall()
	{		
		if (velocity.y >= max_velocity.y)
		{
			++m_free_fall_frame_count;
			if (m_free_fall_frame_count >= 30)
			{
				if (m_is_set_to_keep_falling)
				{
					m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_AT_MAX_FALL });
					mode = Mode::GAMEOVER;
				}
				else
				{					
					velocity.y = 0.0f;
					m_is_echo_update_enabled = false;
					mode = Mode::SUSPENDED;
					m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_SUSPENDED });
				}
			}
		}
		else // keep falling until max velocity reached.
		{
			updateVerticalMovement();
		}
		
		// Keep pushing x velocity towards 0.
		updateHorizontalMovement();
	}


	void Player::updateGameOver()
	{
		m_game_over_timer += m_delta_time;
		if (m_game_over_timer > m_game_over_timer_limit)
		{
			velocity.y = 0.0f;
			m_is_echo_update_enabled = false;
			mode = Mode::SUSPENDED;
			m_game_socket->SendMessage(GameMessage{ GameAction::PLAYER_SUSPENDED });
		}

	}


	void Player::setFrameNormal2Squash()
	{
		if (state == State::ALIVE)
		{
			if (m_squash_frame_seconds > 0.0f)
			{
				m_frame_id = 1;
			}
		}
	}


	void Player::setFrameSquash2Normal()
	{
		if (state == State::ALIVE)
		{
			m_frame_id = 0;
		}
	}
};