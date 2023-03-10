#pragma once

#include <memory>

//#include "key_manager.h"
#include "keyboard_manager.h"

#include "collidable.h"
#include "detector_point.h"

#include "game_socket.h"
#include "glow_socket.h"
#include "camera_socket.h"
#include "collision_socket.h"
#include "life_socket.h"

#include "first_echo.h"


namespace Tmpl8 {

	enum class VectorIndex
	{
		X,
		Y,
	};

	enum class Trigger // Action to trigger wall bounce.
	{
		LEFT,
		RIGHT,
		UP,
	};

	enum class BounceStrength // Strength of the wall bounce.
	{
		WEAK,
		STRONG,
		NONE,
	};
	
	enum class Mode // State the ball is in.
	{
		AIR,
		GROUND,
		WALL,
		CEILING,
		SUSPENDED,
		FREE_FALL,
		GAMEOVER,
		NONE,
	};
	
	class Player : public Collidable
	{
	public:
		// Methods.
		// Structor.
		Player(KeyboardManager& keyboard_manager);
		
		void SetPosition(vec2& start_position);
		void Update(float deltaTime);
		void Draw(Surface* visible_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom, float opacity = 1.0f) override;
		
		void SetTitleScreenMode();
		void SetGameScreenMode();
		void SetStartingLife(int starting_life);
		void TransitionToPosition(vec2& new_position);
		void SetMode(Mode new_mode);

		std::vector<DetectorPoint>& GetCollisionPoints();

		void ResolveCollisions() override;
		
		void UpdatePosition();

		void RegisterGameSocket(Socket<GameMessage>* game_socket);
		void RegisterGlowSocket(Socket<GlowMessage>* glow_socket);
		void RegisterCameraSocket(Socket<CameraMessage>* camera_socket);
		void RegisterLifeSocket(Socket<LifeMessage>* life_socket);
		void RegisterCollisionSocket(Socket<CollisionMessage>* collision_socket);
		
		void RegisterWithCollisionManager();

		int GetStartingLife();
		void EnableEchoUpdate();
		void SetIsTutorialMode(bool is_tutorial_mode) { m_is_tutorial_mode = is_tutorial_mode; }
		void DisableCollisionChecking(bool is_disabled) { m_is_collision_state_disabled = is_disabled; }
		void KeepFalling(bool is_set_to_keep_falling);
		void ToggleDebugMode();

		std::vector<DetectorPoint> points;
		

	private:
		void UpdateCollisionBox();
		bool IsCollisionHandlingDisabled();
		void SetTether(int post_id);
		void SetDeadState();

		// While in air.
		void updateAir();
		void updateVerticalMovement();
		void updateHorizontalMovement();
		void handleGroundCollision();
		void stopBouncing();
		void prepareForGroundMode();
		void SetSquashDelay();
		void handleWallCollision(int post_id);
		void prepareForWallMode(Trigger trigger);
		void handleCeilingCollision();
		void prepareForCeilingMode();
		

		// While on ground.
		void updateGround();
		void bounceOffGround();

		// While on wall.
		void updateWall();
		void bounceOffWall(BounceStrength& wall_bounce_x_power, BounceStrength& wall_bounce_y_power);
		void setEjectionSpeedY(BounceStrength& wall_bounce_y_power);
		void setEjectionSpeedX(BounceStrength& wall_bounce_x_power);

		// While on ceiling.
		void updateCeiling();

		// Special modes.
		void updateFreeFall();
		void updateGameOver();

		// Sprite updates.
		void setFrameNormal2Squash();
		void setFrameSquash2Normal();

		void SetCenterAndBounds();
		float GetDistanceToMove(VectorIndex vector_index, float pre_calculated_t2);

		bool GetIsSafeGlowNeeded(int post_id);
		bool GetIsOnDangerousObstacle(int post_id);

		template <typename T> int GetSign(T val);
		template <typename T> T GetAbsoluteMax(T val1, T val2);


		/* ATTRIBUTES */
		int m_frame_id{ 0 };
		bool m_is_debug_mode_on{ false };

		//CollisionProcessor m_collision_processor{ *this };
		FirstEcho m_player_echo;

		Socket<GameMessage>* m_game_socket{ nullptr };
		Socket<GlowMessage>* m_glow_socket{ nullptr };
		Socket<CameraMessage>* m_camera_socket{ nullptr };
		Socket<LifeMessage>* m_life_socket{ nullptr };
		Socket<CollisionMessage>* m_collision_socket{ nullptr };

		Collidable* m_tethered_object{ nullptr };
		bool m_is_untethering{ false };
		bool m_ignore_tether_collisions{ false };

		/*
			Strength = life and opacity of light flash.
			Buffer is the minimum brightness set by user.
			- Is only decreased during death (no affect on user experience).
		*/
		int m_player_max_strength{ 20 };
		int m_player_min_brightness_buffer{ 10 };
		int m_player_strength{ m_player_max_strength };
		float m_player_brightness_buffer{ static_cast<float>(m_player_min_brightness_buffer) };
		
		// Player sprite.
		Sprite m_sprite;

		// State change tracker.
		Mode mode{ Mode::FREE_FALL };
		State state{ State::DEAD };

		bool m_is_tutorial_mode{ false };
		bool m_has_key_up_happened{ true };

		KeyboardManager& m_keyboard_manager;

		float m_delta_time{ 0.0f };
		float m_dead_timer{ 0.0f };
		float m_dead_time_limit{ 3.0f };
		bool m_is_dead_timer_enabled{ false };
		int m_free_fall_frame_count{ 0 };

		float m_game_over_timer{ 0.0f };
		float m_game_over_timer_limit{ 1.5f };

		bool m_allow_horizontal_movement{ true };
		float m_horizontal_dead_zone{ 0.3f };

		// Movement related.
		float m_ground_bounce_power{ 4.0f };
		float m_max_ground_bounce_power{ m_ground_bounce_power };
		float m_ground_bounce_power_dead_zone{ 0.0f };

		vec2 acceleration{ 25.0f, 5.0f };
		vec2 magnitude_coefficient{ 10.0f, 48.0f };
		float m_acceleration_x_dead_dampening{ 0.1f };

		std::vector<Collidable*> collisions;

		vec2 prev_center{ 0.0f, 0.0f };

		vec2 position{ 640.0f, 360.0f };
		vec2 prev_position{ 0.0f, 0.0f };
		vec2 distance{ 0.0f, 0.0f };

		vec2 velocity{ 0.0f, 0.0f };
		vec2 max_velocity{ 10.0f, 10.0f }; // x component is increased for strong wall bounce.
		float maxSpeedNormalX{ max_velocity.x }; // Used to reset max_velocity.x.
		
		vec2 direction{ 0.0f , 0.0f }; // Only x component is used.

		// Wall bounce related.
		Trigger wallBounceTrigger{}; // What direction to press to do strong bounce.
		BounceStrength wall_bounce_y_power{}; // Strength of wall bounce.
		BounceStrength wall_bounce_x_power{};	

		// Frame counts determine how long certain conditions last.
		float m_squash_frame_seconds{ 0 };		

		bool m_is_vertical_at_rest{ false };
		bool m_is_horizontal_at_rest{ false };		

		bool m_is_echo_update_enabled{ true };
		bool m_is_at_finish_line{ false };		
		int m_life_restored_by_pickup{ 5 };

		bool m_is_collision_state_disabled{ false };
		bool m_is_set_to_keep_falling{ false };
	};
}