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
		screen{ surface },		
		level_manager{ text_repo },
		player{ leftKey, rightKey, upKey, downKey },
		camera{ player },
		viewport{ surface, camera },
		collision_manager{ player, camera }
	{	}

	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		RegisterSockets();
		InitLevelManager();
		InitGlowManager();
		InitPlayer();

		PrepareForNextLevel();

		player.RestoreDefaults();
		player.SetPosition(level_manager.GetPlayerStartPosition());
	}
	

	void Game::RegisterSockets()
	{
		m_collision_socket = collision_manager.GetCollisionSocket();
		m_glow_socket = glow_manager.GetGlowSocket();
		m_camera_socket = camera.GetCameraSocket();
		m_viewport_socket = viewport.GetViewportSocket();
		m_life_socket = viewport.GetLifeHUDSocket();
	}


	void Game::InitLevelManager()
	{
		level_manager.RegisterGlowSocket(m_glow_socket);
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
	}


	void Game::PrepareCollisionManager()
	{		
		collision_manager.SetNewLevel(level_manager);
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
		// Convert to seconds. If very slow comp, process at ~30 FPS minimum.
		// Unlikely to break anything if larger deltaTime, but experience gets weird.
		deltaTime = Clamp(deltaTime * 0.001f, 0.0f, 0.033f);

		// See if anything has sent a message to game. Usually about player death or level completion.
		CheckSocketForNewMessages();

		/*
			Move player and check for collisions.
			- Reposition if in obstacle.
			- Trigger creation of new glow orb.
			- Set new mode (ground/wall/ceiling).		
		*/
		player.Update(deltaTime);
		collision_manager.UpdateCollisions(CollidableGroup::PLAYER);

		// Update Level (move pickups up and down).
		level_manager.Update(deltaTime);

		// Update GlowOrbs (destroy old, create new, update sizes).
		glow_manager.Update(deltaTime);

		/*
			Follow player if moved too far from center.
			- Objects only drawn to screen if they collide with the viewport.
		*/
		viewport.Update(deltaTime);
		collision_manager.UpdateCollisions(CollidableGroup::CAMERA); // MAKE COLLISION MANAGER A BASE CLASS. CAMERA AND PLAYER WILL BE SUBCLASSES.

		/*
			1. Draw objects the camera sees (glow orbs and pickups).
			2. Draw player on top of everything else.
			3. Draw rest of game overlay (HUD, pause menu, etc).
		*/
		viewport.Draw(deltaTime);	// SHOULD HOLD ALL SURFACES? OR CREATE SEPARATE SURFACE DATA CLASS?
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
			ResetPlayerPosition();
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
			// Display transition message.
			// See core memory.
			// Award new powers.
			// - There would be a new GameMessage/Action after these are complete.
			// - We would move this action to happen after that trigger.
			ReadyNextLevel();			
			m_is_in_level_advancement = false;
			m_level_action_tracker = 0;
			break;
		}
	}


	void Game::CheckMessage(GameMessage& message)
	{
		switch (message.m_action)
		{
		case GameAction::PLAYER_DEATH:
			m_is_in_level_reset = true;
			FadeToBlack();
			break;
		case GameAction::LEVEL_COMPLETE:
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
		glow_manager.TriggerSafeOrbDestruction();
		camera.FadeToBlack();
	}
	

	// Let player begine falling as part of transition.
	void Game::DisablePlayerCollisions()
	{
		// 'Orbs removed' and 'player in free-fall' must both happen first.
		if (m_level_action_tracker == 2)
		{
			collision_manager.EnablePlayerCollisions(false);
		}
	}


	// Place player at level start. It will appear as if player landed there directly from where they fell off from earlier.
	void Game::ResetPlayerPosition()
	{
		player.RestoreDefaults();
		player.TransitionToPosition(level_manager.GetPlayerStartPosition());
		camera.SetPosition(level_manager.GetPlayerStartPosition());
		camera.FadeIntoView();
		collision_manager.EnablePlayerCollisions(true);
	}


	// -----------------------------------------------------------
	// Go to next level
	// -----------------------------------------------------------

	void Game::ReadyNextLevel()
	{
		if (++level_id > 3)
		{
			// Game completed.
		}
		else
		{
			PrepareForNextLevel();
			ResetPlayerPosition();
		}
	}

	// -----------------------------------------------------------
	// Keyboard events
	// -----------------------------------------------------------

	// MAKE CLASS THAT CAN BE PASSED TO PLAYER AS REF FROM GAME.

	/* Register key press only if the key was not already being pressed
		(must register key release first). */

	void Game::KeyDown(int key)
	{
		switch (key)
		{
		case 80: // LEFT.
			if (!leftKey.isPressed)
			{
				leftKey.isActive = true;
				leftKey.isPressed = true;
				rightKey.isActive = false;
			}
			break;
		case 79: // RIGHT.
			if (!rightKey.isPressed)
			{
				rightKey.isActive = true;
				rightKey.isPressed = true;
				leftKey.isActive = false;
			}
			break;
		case 82: // UP.
			if (!upKey.isPressed)
			{
				upKey.isActive = true;
				upKey.isPressed = true;
				downKey.isActive = false;
			}
			break;
		case 81: // DOWN.
			if (!downKey.isPressed)
			{
				downKey.isActive = true;
				downKey.isPressed = true;
				upKey.isActive = false;
			}
			break;
		}
	}

	void Game::KeyUp(int key)
	{
		switch (key)
		{
		case 80: // LEFT.
			leftKey.isActive = false;
			leftKey.isPressed = false;
			break;
		case 79: // RIGHT.
			rightKey.isActive = false;
			rightKey.isPressed = false;
			break;
		case 82: // UP.
			upKey.isActive = false;
			upKey.isPressed = false;
			break;
		case 81: // DOWN.
			downKey.isActive = false;
			downKey.isPressed = false;
			break;
		}
	}
};