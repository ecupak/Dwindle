#pragma once

#include "template.h"
#include "surface.h"

#include "key_manager.h"
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
		
		void Init();
		void Shutdown();
		void Tick(float deltaTime);
		void MouseUp(int button) {};
		void MouseDown(int button) {};
		void MouseMove(int x, int y) {};
		void KeyUp(int key_code);
		void KeyDown(int key_code);

	private:
		void RegisterKeys();
		void RegisterSockets();
		void InitLevelManager();
		void InitGlowManager();
		void InitPlayer();
		void InitCamera();
		void InitCollisionManager();

		void PrepareForNextLevel();
		void PrepareLevel();
		void PrepareGlowManager();
		void PreparePlayer();
		void PrepareCollisionManager();
		void PrepareCamera();

		void CheckSocketForNewMessages();
		void ProcessMessages();
		void CheckLevelResetProgress(GameMessage& messages);
		void CheckLevelAdvancementProgress(GameMessage& messages);
		void CheckMessage(GameMessage& messages);

		void FadeToBlack();
		void DisablePlayerCollisions();
		void ResetPlayerPosition();

		void ReadyNextLevel();


		// ATTRIBUTES
		/* 
			Core classes.
			Leave in order for initialization.
		*/
		CollisionManager collision_manager;
		TextRepo text_repo{};
		Level level_manager;
		KeyManager key_manager;
		Player player;
		Camera camera;
		Surface* screen{ nullptr };
		Viewport viewport;
		GlowManager glow_manager{};
		
		/*
			Sockets.
			Handles communication among classes.
		*/
		Socket<GameMessage> m_game_hub;
		Socket<GlowMessage>* m_glow_socket{ nullptr };
		Socket<CollisionMessage>* m_collision_socket{ nullptr };
		Socket<CameraMessage>* m_camera_socket{ nullptr };
		Socket<ViewportMessage>* m_viewport_socket{ nullptr };
		Socket<LifeMessage>* m_life_socket{ nullptr };

		int level_id{ 0 };
		int m_free_fall_frames{ 0 };

		int m_level_action_tracker{ 0 };
		bool m_is_in_level_reset{ false };
		bool m_is_in_level_advancement{ false };
	};
};