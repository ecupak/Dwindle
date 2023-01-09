#include <cstdio> //printf
#include <vector>

#include "game.h"
#include <SDL2-2.0.3/include/SDL_mouse.h>


namespace Tmpl8
{
	// -----------------------------------------------------------
	// Constant
	// -----------------------------------------------------------


	// -----------------------------------------------------------
	// Constructor
	// -----------------------------------------------------------
	Game::Game(Surface* surface) :
		collision_manager{ },
		level_manager{ text_repo },
		player{ key_manager },
		camera{ player },
		screen{ surface },
		viewport{ surface, camera, game_mode, mouse_manager }
	{	}

	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------	
	void Game::Init()
	{
		RegisterKeys();
		RegisterSockets();
		InitLevelManager();
		InitGlowManager();
		InitPlayer();
		InitCamera();
		InitCollisionManager();
		InitViewport();

		PrepareForNextLevel();

		camera.SetCenter(vec2{ ScreenWidth / 2, ScreenHeight / 2 });
		player.SetPosition(level_manager.GetPlayerStartPosition());
		player.SetTitleScreenMode();
		//camera.FadeIntoView(0.0f);
	}


	void Game::RegisterKeys()
	{
		key_manager.RegisterKey(SDLK_LEFT);
		key_manager.RegisterKey(SDLK_RIGHT);
		key_manager.RegisterKey(SDLK_UP);
		key_manager.RegisterKey(SDLK_DOWN);
		key_manager.RegisterKey(SDLK_F1);
	}


	void Game::RegisterSockets()
	{
		m_collision_socket = collision_manager.GetCollisionSocket();
		m_glow_socket = glow_manager.GetGlowSocket();
		m_camera_socket = camera.GetCameraSocket();
		m_life_socket = viewport.GetLifeHUDSocket();
	}


	void Game::InitLevelManager()
	{
		level_manager.RegisterGlowSocket(m_glow_socket);
		level_manager.RegisterCollisionSocket(m_collision_socket);
	}


	void Game::InitGlowManager()
	{
		glow_manager.RegisterGameSocket(&m_game_hub);
		glow_manager.RegisterCollisionSocket(m_collision_socket);
	}


	void Game::InitPlayer()
	{
		player.RegisterGameSocket(&m_game_hub);
		player.RegisterGlowSocket(m_glow_socket);
		player.RegisterCameraSocket(m_camera_socket);
		player.RegisterLifeSocket(m_life_socket);
		player.RegisterCollisionSocket(m_collision_socket);
	}

	void Game::InitCamera()
	{
		camera.RegisterCollisionSocket(m_collision_socket);
	}


	void Game::InitCollisionManager()
	{
		player.RegisterWithCollisionManager();
		camera.RegisterWithCollisionManager();
	}

	void Game::InitViewport()
	{
		viewport.SetGameSocket(&m_game_hub);
		viewport.CreateButtons();
	}

	// -----------------------------------------------------------
	// Set up the next level
	// -----------------------------------------------------------

	void Game::PrepareForNextLevel()
	{
		PrepareLevel();
		PrepareGlowManager();
		PreparePlayer();
		PrepareCollisionManager();
		PrepareCamera();
	}


	void Game::PrepareLevel()
	{
		level_manager.CreateLevel(level_id); // starts at 0 = tutorial.		
	}


	void Game::PrepareGlowManager()
	{
		glow_manager.SetMapLayer(level_manager.GetMapLayer());
		glow_manager.SetObstacleLayer(level_manager.GetObstacleLayer());
	}


	void Game::PreparePlayer()
	{
		player.SetIsTutorialMode(level_id == 0);
		player.SetStartingLife(level_manager.GetPlayerStartingLife());
	}


	void Game::PrepareCollisionManager()
	{
		collision_manager.PopulateLists();
	}


	void Game::PrepareCamera()
	{
		camera.SetRevealedLayer(level_manager.GetRevealedLayer());
		camera.SetLevelBounds(level_manager.GetBounds());
	}

	// -----------------------------------------------------------
	// Close down application
	// -----------------------------------------------------------
	void Game::Shutdown()
	{}


	// -----------------------------------------------------------
	// Main application tick function
	// -----------------------------------------------------------	
	void Game::Tick(float deltaTime)
	{
		deltaTime = Clamp(deltaTime * 0.001f, 0.0f, 0.033f);

		mouse_manager.Update(m_mouse_packet);

		// See if anything has sent a message to game (player death or level completion).
		CheckSocketForNewMessages();

		// Move camera (child of viewport) to follow player.
		viewport.Update(deltaTime);

		// Update Level (pickup animation and moving obstacles).
		level_manager.Update(deltaTime);

		// User input moves player - may be clipping an object at this point.
		player.Update(deltaTime);

		// Update GlowOrbs (destroy old, create new, update sizes, etc).
		glow_manager.Update(deltaTime);

		// Do collisions in one pass. This will get player out of walls and figure out what the camera can see / will draw.
		collision_manager.RunCollisionCycle();

		// Render whatever the camera has collided with based on its new position.
		viewport.Draw(deltaTime);
	}

	// -----------------------------------------------------------
	// Game socket/messaging check
	// -----------------------------------------------------------	
	void Game::CheckSocketForNewMessages()
	{
		if (m_game_hub.HasNewMessage())
		{
			ProcessMessages();
		}
	}


	void Game::ProcessMessages()
	{
		std::vector<GameMessage> messages = m_game_hub.ReadMessages();
		m_game_hub.ClearMessages();

		for (GameMessage& message : messages)
		{
			if (m_is_in_level_reset)
			{
				CheckLevelResetProgress(message);
			}
			else if (m_is_in_level_advancement)
			{
				CheckLevelAdvancementProgress(message);
			}
			else if (m_is_in_title_screen)
			{
				CheckTitleScreenProgress(message);
			}
			else
			{
				CheckMessage(message);
			}
		}
	}


	void Game::CheckLevelResetProgress(GameMessage& message)
	{
		switch (message.m_action)
		{
		case GameAction::ORBS_REMOVED:
			++m_level_action_tracker;
			DisablePlayerCollisions();
			break;
		case GameAction::PLAYER_IN_FREE_FALL:
			++m_level_action_tracker;
			DisablePlayerCollisions();
			break;
		case GameAction::PLAYER_SUSPENDED:
			ReadyNextLevel();
			m_is_in_level_reset = false;
			m_level_action_tracker = 0;
			break;
		}
	}


	void Game::CheckLevelAdvancementProgress(GameMessage& message)
	{
		switch (message.m_action)
		{
		case GameAction::ORBS_REMOVED:
			++m_level_action_tracker;
			DisablePlayerCollisions();
			break;
		case GameAction::PLAYER_IN_FREE_FALL:
			++m_level_action_tracker;
			DisablePlayerCollisions();
			break;
		case GameAction::PLAYER_SUSPENDED:
			AdvanceLevel();
			ReadyNextLevel();
			m_is_in_level_advancement = false;
			m_level_action_tracker = 0;
			break;
		}
	}


	void Game::CheckTitleScreenProgress(GameMessage& message)
	{
		switch (message.m_action)
		{
		case GameAction::START_TUTORIAL:
			++m_level_action_tracker;
			LeaveTitleToLevel(0);
			break;
		case GameAction::START_GAME:
			++m_level_action_tracker;
			LeaveTitleToLevel(1);
			break;
		case GameAction::PLAYER_SUSPENDED:
			game_mode = GameMode::GAME_SCREEN;
			ReadyNextLevel();
			m_is_in_title_screen = false;
			m_level_action_tracker = 0;
			break;
		}
	}


	void Game::CheckMessage(GameMessage& message)
	{
		switch (message.m_action)
		{		
		case GameAction::PLAYER_DEATH:
			m_level_action_tracker = 0;
			m_is_in_level_reset = true;
			FadeToBlack();
			break;
		case GameAction::LEVEL_COMPLETE:
			m_level_action_tracker = 0;
			m_is_in_level_advancement = true;
			FadeToBlack();
			break;
		}
	}


	// -----------------------------------------------------------
	// Reset Level
	// -----------------------------------------------------------

	// Remove all safe glow orbs. Screen should be completely black during transition.
	void Game::FadeToBlack()
	{
		glow_manager.TriggerStaticOrbDestruction();
		camera.FadeToBlack();
		level_manager.FadeToBlack();
	}


	// Let player begine falling as part of transition.
	void Game::DisablePlayerCollisions()
	{
		// 'Orbs removed' and 'player in free-fall' must both happen first.
		if (m_level_action_tracker == 2)
		{
			player.DisableCollisionChecking(true);
		}
	}


	// Place player at level start. It will appear as if player landed there directly from where they fell off from earlier.
	void Game::ResetPlayerPosition()
	{
		player.SetGameScreenMode();
		player.TransitionToPosition(level_manager.GetPlayerStartPosition());
		camera.SetCenter(level_manager.GetPlayerStartPosition());
		camera.RestoreView();
		player.DisableCollisionChecking(false);
	}


	// -----------------------------------------------------------
	// Go to next level
	// -----------------------------------------------------------
	void Game::AdvanceLevel()
	{
		++level_id;
	}
	
	void Game::ReadyNextLevel()
	{			
		PrepareForNextLevel();
		ResetPlayerPosition();
	}

	// -----------------------------------------------------------
	// Title Screen
	// -----------------------------------------------------------	
	void Game::LeaveTitleToLevel(int starting_level_id)
	{
		if (m_level_action_tracker == 1)
		{
			level_id = starting_level_id;
			player.SetMode(Mode::FREE_FALL);
		}
	}

	// -----------------------------------------------------------
	// Mouse events
	// -----------------------------------------------------------
	void Game::MouseUp(int button)
	{
		if (button == SDL_BUTTON_LEFT)
		{
			m_mouse_packet.m_was_pressed = true;
		}
	}

	void Game::MouseDown(int button)
	{
		if (button == SDL_BUTTON_LEFT)
		{
			m_mouse_packet.m_was_pressed = false;
		}
	}

	void Game::MouseMove(int x, int y)
	{
		m_mouse_packet.m_x = x;
		m_mouse_packet.m_y = y;
	}


	// -----------------------------------------------------------
	// Keyboard events
	// -----------------------------------------------------------

	/* Register key press only if the key was not already being pressed
		(must register key release first). */

	void Game::KeyDown(int key_code)
	{
		KeyInfo& key = key_manager.GetKey(key_code);
		
		if (key.m_is_pressed == false)
		{
			key.m_is_active = true;
			key.m_is_pressed = true;

			switch (key_code)
			{
			case SDLK_LEFT:
				key_manager.GetKey(SDLK_RIGHT).m_is_active = false;
				break;
			case SDLK_RIGHT:
				key_manager.GetKey(SDLK_LEFT).m_is_active = false;
				break;
			case SDLK_UP:
				key_manager.GetKey(SDLK_DOWN).m_is_active = false;
				break;
			case SDLK_DOWN:
				key_manager.GetKey(SDLK_UP).m_is_active = false;
				break;
			case SDLK_F1:
				player.ToggleDebugMode();
				break;
			default:
				break;
			}
		}
	}

	void Game::KeyUp(int key_code)
	{
		KeyInfo& key = key_manager.GetKey(key_code);

		key.m_is_active = false;
		key.m_is_pressed = false;
	}
};