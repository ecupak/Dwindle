#pragma once

#include "collidable.h"
#include "detector_point.h"
#include "key_state.h"
#include "glow_socket.h"
#include "camera_socket.h"

namespace Tmpl8 {
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
		REST,
		NONE,
	};

	class Player : public Collidable
	{
	public:
		// Methods.
		// Structor.
		Player(Surface* screen, keyState& leftKey, keyState& rightKey, keyState& upKey, keyState& downKey);
		
		void Update(float deltaTime);
		void Draw(Surface* viewable_layer, int c_left, int c_top, int in_left, int in_top, int in_right, int in_bottom) override;
		void SetPosition(vec2& start_position);
		
		std::vector<DetectorPoint>& GetCollisionPoints();
		virtual void ResolveCollision(Collidable*& collision);
		void ProcessCollisions();
		
		// Point method test.
		std::vector<DetectorPoint> points;
		void UpdatePosition();

		void RegisterGlowSocket(Socket<GlowMessage>& glow_socket);
		void RegisterCameraSocket(Socket<CameraMessage>& camera_socket);

		float GetDistanceToBounceApex();


		int safe_glows_created{ 0 };

	private:
		/* METHODS */
		void UpdateCollisionBox();

		// While in air.
		void updateAir();
		void updateVerticalMovement();
		void updateHorizontalMovement();
		void handleGroundCollision();
		void stopBouncing();
		void prepareForGroundMode();
		void setSquashFrameCount();
		void setStretchFrameCount();
		void handleWallCollision(int post_id);
		void prepareForWallMode(Trigger trigger);
		void handleCeilingCollision();
		void prepareForCeilingMode();

		void updateCeiling();

		// While on ground.
		void updateGround();
		void bounceOffGround();

		// While on wall.
		void updateWall();
		void bounceOffWall(BounceStrength& wall_bounce_x_power, BounceStrength& wall_bounce_y_power);
		void setEjectionSpeedY(BounceStrength& wall_bounce_y_power);
		void setEjectionSpeedX(BounceStrength& wall_bounce_x_power);

		// Sprite updates.
		void updateFrameStretch2Normal();
		void updateFrameDirectionLockRelease();
		void setFrameNormal2Squash();
		void setFrameSquash2Stretch();
		void setFrameAfterWallBounce(bool isWeakBounce);

		void SetCenterAndBounds();
		float GetDistanceToMove(int vec2_index, float pre_calculated_t2);

		bool GetIsSafeGlowNeeded(int post_id);

		template <typename T> int GetSign(T val);
		template <typename T> T GetAbsoluteMax(T val1, T val2);


		/* ATTRIBUTES */

		Socket<GlowMessage>* m_glow_socket{ nullptr };
		Socket<CameraMessage>* m_camera_socket{ nullptr };

		// Screen reference.
		Surface* m_screen;

		// Player sprite.
		Sprite m_sprite;
		int half_height;
		int half_width;
		int half_size;

		// State change tracker.
		Mode mode{ Mode::AIR };

		keyState& m_leftKey;
		keyState& m_rightKey;
		keyState& m_upKey;
		keyState& m_downKey;

		float m_delta_time{ 0.0f };

		// Movement related.
		float ground_bounce_power{ 4.0f };
		vec2 acceleration{ 25.0f, 5.0f };
		vec2 magnitude_coefficient{ 10.0f, 48.0f };

		std::vector<Collidable*> collisions;

		vec2 prev_center{ 0.0f, 0.0f };

		vec2 position{ 20.0f, 200.0f };
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

		// Intrinsic properties.
		//float elasticity{ 1.0f }; // Anything less than 1 and ball loses velocity with each bounce.
		float squashDampeningMagnitude{ 0.5f }; // Subtracts from squash value to get squash duration.
		float squashDampeningCoefficient{ 0.25f }; // Multiplies by squash value to get squash duration.
		float deadZone{ 0.0f }; //{ acceleration.y * 0.4f }; // When velocity.y is less than this, ball stops bouncing.

		// Frame counts determine how long certain conditions last.
		//int directionLockedFrameCount{ 0 };
		float squashFrameCount{ 0 };
		int stretchFrameCount{ 0 };
		float triggerFrameCount{ 0 };
	};




}