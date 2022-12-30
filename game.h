#pragma once

#include "template.h"
#include "surface.h"

#include "key_state.h"
#include "player.h"
#include "level.h"
#include "text_repo.h"
#include "glow_manager.h"
#include "collision_manager.h"
#include "viewport.h"
#include "camera.h"

#include "game_socket.h"

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
		void CheckSocketForNewMessages();
		void ProcessMessages();

		void RegisterSockets();
		void PrepareTextRepo();
		void PrepareLevel();
		void PrepareGlowManager();
		void PreparePlayer();
		void PrepareCollisionManager();		
		void PrepareCamera();

		void FadeToBlack();
		void DisablePlayerCollisions();
		void ResetPlayerPosition();

		// ATTRIBUTES
		// Core classes.
		Surface* screen{ nullptr };
		Player player;
		Camera camera;		
		Viewport viewport;
		CollisionManager collision_manager;
		GlowManager glow_manager;
		Level level_manager;
		TextRepo text_repo;
				
		keyState leftKey;
		keyState rightKey;
		keyState upKey;
		keyState downKey;

		Socket<GameMessage> m_game_hub;
		Socket<GlowMessage>* m_glow_socket;
		Socket<CollisionMessage>* m_collision_socket;
		Socket<CameraMessage>* m_camera_socket;
		Socket<ViewportMessage>* m_viewport_socket;
		Socket<LifeMessage>* m_life_socket;

		int level_id{ 1 };
		int m_free_fall_frames{ 0 };

		int m_level_reset_step{ 0 };
	};
};