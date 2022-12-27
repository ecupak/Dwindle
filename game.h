#pragma once

#include "key_state.h"
#include "template.h"
#include "level.h"
#include "player.h"
#include "viewport.h"
#include "collision_manager.h"
#include "camera.h"

namespace Tmpl8
{
	class Game
	{
	public:
		Game::Game(Surface* surface);
		//void SetTarget( Surface* surface ) { screen = surface; }
		void Init();
		void Shutdown();
		void Tick(float deltaTime);
		void MouseUp(int button) {};
		void MouseDown(int button) {};
		void MouseMove(int x, int y) {};
		void KeyUp(int key);
		void KeyDown(int key);

	private:		
		void PrepareLevel();
		void PreparePlayer();
		void PrepareCollisionManager();		
		void PrepareCamera();

		// ATTRIBUTES
		// Core classes.
		Surface* screen{ nullptr };
		Player player;
		Camera camera;		
		Viewport viewport;
		CollisionManager collision_manager;
		Level level;
				
		keyState leftKey;
		keyState rightKey;
		keyState upKey;
		keyState downKey;


		int level_id{ 1 };

		bool startloop{ false };
		int loop_switch{ -1 };
	};
};