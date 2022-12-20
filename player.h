#pragma once

#include "collidable.h"
#include "detector_point.h"
#include "key_state.h"
#include "template.h"
#include "surface.h"
#include "glow_socket.h"

namespace Tmpl8 {
	enum class Trigger // Action to trigger wall bounce.
	{
		LEFT,
		RIGHT,
	};

	enum class BounceStrength // Strength of the wall bounce.
	{
		WEAK,
		STRONG,
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
		Player(Surface* screen);
		
		void Update(float deltaTime, keyState& leftKey, keyState& rightKey, keyState& upKey, keyState& downKey);
		void Draw(Surface* screen);
		void SetPosition(vec2& start_position);
		
		std::vector<DetectorPoint>& GetCollisionPoints();
		virtual void ResolveCollision(Collidable*& collision);
		void ProcessCollisions();
		
		// Point method test.
		std::vector<DetectorPoint> points;
		std::vector<DetectorPoint*> point_ptrs;
		void SetPointPositions();
		void UpdatePointPositions();

		void RegisterGlowSocket(GlowSocket& glow_socket);

	private:
		/* METHODS */

		// While in air.
		void updateAir(keyState& leftKey, keyState& rightKey);
		void updateVerticalMovement();
		void updateHorizontalMovement(keyState& leftKey, keyState& rightKey);
		void updateHorizontalMovementSpeed(keyState& leftKey, keyState& rightKey);
		void handleGroundCollision();
		void stopBouncing();
		void prepareForGroundMode();
		void setSquashFrameCount();
		void setStretchFrameCount();
		void handleWallCollision(std::vector<int>& posts);
		void prepareForWallMode(Trigger trigger);
		void handleCeilingCollision();
		void prepareForCeilingMode();

		void updateCeiling();

		// While on ground.
		void updateGround(keyState& leftKey, keyState& rightKey);
		void bounceOffGround();

		// While on wall.
		void updateWall(keyState& leftKey, keyState& rightKey);
		void bounceOffWall(BounceStrength bounceStrength);
		void setEjectionSpeed(bool isWeakBounce);
		void setDirectionLockFrameCount(bool isWeakBounce);
		void setEjectionSpeedY(bool isWeakBounce);
		void setEjectionSpeedX(bool isWeakBounce);

		// Sprite updates.
		void updateFrameStretch2Normal();
		void updateFrameDirectionLockRelease();
		void setFrameNormal2Squash();
		void setFrameSquash2Stretch();
		void setFrameAfterWallBounce(bool isWeakBounce);


		template <typename T> int GetSign(T val);
		template <typename T> T GetAbsoluteMax(T val1, T val2);


		/* ATTRIBUTES */

		GlowSocket* m_glow_socket;

		// Screen reference.
		Surface* m_screen;

		// Player sprite.
		Sprite m_sprite;
		int half_height;
		int half_width;
		int half_size;

		// State change tracker.
		Mode phase{ Mode::NONE };


		float m_delta_time{ 0.0f };

		// Movement related.
		float ground_bounce_power{ 6.0f };

		std::vector<Collidable*> collisions;

		vec2 prev_center{ 0.0f, 0.0f };

		vec2 position{ 20.0f, 200.0f };
		vec2 prev_position{ 0.0f, 0.0f };

		vec2 i_position{ 0.0f, 0.0f };
		vec2 i_prev_position{ 0.0f, 0.0f };

		vec2 hiddenPos{ 0.0f, 0.0f }; // Only y component is used; cheats the ball landing on the ground.
		vec2 speed{ 0.0f, 0.0f };
		vec2 maxSpeed{ 1.5f, 10.0f }; // x component is increased for strong wall bounce.
		float maxSpeedNormalX{ maxSpeed.x }; // Used to reset maxSpeed.x.
		
		vec2 acceleration{ 5.0f, 10.0f };
		
		vec2 direction{ 0.0f , 0.0f }; // Only x component is used.

		// Wall bounce related.
		Trigger wallBounceTrigger{}; // What direction to press to do strong bounce.
		BounceStrength wallBounceStrength{}; // Strength of wall bounce.

		// Intrinsic properties.
		float elasticity{ 1.0f }; // Anything less than 1 and ball loses speed with each bounce.
		float squashDampeningMagnitude{ 0.5f }; // Subtracts from squash value to get squash duration.
		float squashDampeningCoefficient{ 0.25f }; // Multiplies by squash value to get squash duration.
		float deadZone{ 0.0f }; //{ acceleration.y * 0.4f }; // When speed.y is less than this, ball stops bouncing.

		// Frame counts determine how long certain conditions last.
		int directionLockedFrameCount{ 0 };
		float squashFrameCount{ 0 };
		int stretchFrameCount{ 0 };
		float triggerFrameCount{ 0 };

		// Collision helpers.
		bool isTouchingSide{ false };
		bool isTouchingGround{ false };

		// Boundaries.
		int ground{ ScreenHeight };
		int leftWall{ 0 };
		int rightWall{ ScreenWidth };
	};




}