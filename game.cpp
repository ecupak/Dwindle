#include <cstdio> //printf
#include <vector>

#include "game.h"



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
		player{ m_keyboard_manager },
		camera{ player },
		screen{ surface },
		viewport{ surface, camera, game_mode, m_mouse_manager }
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
		/*key_manager.RegisterKey(SDLK_LEFT);
		key_manager.RegisterKey(SDLK_RIGHT);
		key_manager.RegisterKey(SDLK_UP);
		key_manager.RegisterKey(SDLK_DOWN);
		key_manager.RegisterKey(SDLK_F1);*/
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
		// -1 = title
		// 0 = tutorial
		// 1 - 3 = levels
		// 4 = end
		level_manager.CreateLevel(level_id);
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

		m_mouse_manager.Update();
		m_keyboard_manager.Update();

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
			else if (m_is_in_game_over)
			{
				CheckGameOverProgress(message);
			}
			else if (m_is_in_return_to_title)
			{
				CheckReturnToTitleProgress(message);
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
			LeaveTitleToLevel(3);
			break;
		case GameAction::PLAYER_SUSPENDED:
			game_mode = GameMode::GAME_SCREEN;
			ReadyNextLevel();
			m_is_in_title_screen = false;
			m_level_action_tracker = 0;
			break;
		}
	}


	void Game::CheckGameOverProgress(GameMessage& message)
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
			player.KeepFalling(true);
			break;
		case GameAction::PLAYER_AT_MAX_FALL:
			++m_level_action_tracker;
			PrepareForEnding();
			break;
		case GameAction::PLAYER_SUSPENDED:
			++m_level_action_tracker;
			DisplayEnding();
			m_is_in_game_over = false;
			m_level_action_tracker = 0;
			break;
		}
	}


	void Game::CheckReturnToTitleProgress(GameMessage& message)
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
			++m_level_action_tracker;
			DisplayTitleScreen();
			m_is_in_return_to_title = false;
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
			AdvanceLevel();			
			FadeToBlack();
			break;
		case GameAction::TUTORIAL_COMPLETE:
			m_level_action_tracker = 0;
			m_is_in_return_to_title = true;
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
		camera.FadeIn(1.0f);
		player.DisableCollisionChecking(false);
	}


	// -----------------------------------------------------------
	// Go to next level
	// -----------------------------------------------------------
	void Game::AdvanceLevel()
	{
		if (++level_id > 3)
		{			
			m_is_in_game_over = true;
		}
		else
		{
			m_is_in_level_advancement = true;
		}
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


	void Game::DisplayTitleScreen()
	{
		/*
			freeze player			
			recenter (camera follows)
			turn on echoes (keep velocity 0)

			fade title and buttons into view
			done?
		*/

		level_id = -1; // Title.



	}


	// -----------------------------------------------------------
	// Game ending
	// -----------------------------------------------------------
	void Game::PrepareForEnding()
	{
		game_mode = GameMode::GAME_OVER;
		camera.FreezeView();
		PrepareForNextLevel();
	}


	void Game::DisplayEnding()
	{
		camera.MoveToCenter();
		camera.FadeIn(0.0f);
	}


	// -----------------------------------------------------------
	// Mouse events
	// -----------------------------------------------------------
	void Game::MouseDown(int button)
	{
		m_mouse_manager.Pressed(button);
	}


	void Game::MouseUp(int button)
	{
		m_mouse_manager.Released(button);
	}	


	void Game::MouseMove(int x, int y)
	{
		if (game_mode == GameMode::TITLE_SCREEN)
		{
			m_mouse_manager.SetPosition(x, y);
		}
	}


	// -----------------------------------------------------------
	// Keyboard events
	// -----------------------------------------------------------

	/* Register key press only if the key was not already being pressed
		(must register key release first). */

	void Game::KeyDown(int key_code)
	{
		m_keyboard_manager.Pressed(key_code);
	}

	void Game::KeyUp(int key_code)
	{
		m_keyboard_manager.Released(key_code);
	}
};