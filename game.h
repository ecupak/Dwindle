#pragma once

#include "key_state.h"
#include "template.h"
#include "level.h"
#include "player.h"
#include "viewport.h"
#include "collision_manager.h"

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
		void SetPlayerStartPosition();

		void PreparePlayer();
		void PrepareCollisionManager();
		void RegisterPlayerGlowSocket();

		void PrepareLevel();
		void CreateLevel();
		void RegisterLevelCollisionSocket();

		void PrepareViewport();

		// ATTRIBUTES
		// Core classes.
		Level level;
		Player player;
		Viewport viewport;

		CollisionManager collision_manager;

		Surface* screen{ nullptr };
		keyState leftKey;
		keyState rightKey;
		keyState upKey;
		keyState downKey;


		int level_id{ 1 };

		bool startloop{ false };
		int loop_switch{ -1 };
	};
};