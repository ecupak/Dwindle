#include <cstdio> //printf
#include <vector>

#include "game.h"
#include "surface.h"
#include "level.h"


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
		//level{ },
		player{ surface, leftKey, rightKey, upKey, downKey },
		//viewport{ },
		collision_manager{ viewport, player }
	{	}

	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		// Container class tracks level.
		PrepareLevel();
		PreparePlayer();
		PrepareCollisionManager();
		PrepareViewport();
	}


	void Game::PrepareLevel()
	{
		CreateLevel();
		RegisterLevelCollisionSocket();
	}

	void Game::CreateLevel()
	{
		level_id = 1;
		level.CreateLevel(level_id); // starts at 1.
	}


	void Game::RegisterLevelCollisionSocket()
	{
		level.RegisterCollisionSocket(collision_manager.GetLevelCollisionSocket());
		level.RegisterCollisionSocketToGlowManager(collision_manager.GetGlowCollisionSocket());
	}


	void Game::PreparePlayer()
	{
		SetPlayerStartPosition();
		RegisterPlayerGlowSocket();
	}


	void Game::SetPlayerStartPosition()
	{
		player.SetPosition(level.GetPlayerStartPosition());
	}


	void Game::RegisterPlayerGlowSocket()
	{
		player.RegisterGlowSocket(level.GetPlayerGlowSocket());
	}


	void Game::PrepareCollisionManager()
	{
		collision_manager.SetNewLevel(level);

	}


	void Game::PrepareViewport()
	{
		viewport.SetBackgroundLayer(level.GetBackgroundLayer());
		viewport.SetObstacleLayer(level.GetObstacleLayer());
		viewport.SetMapLayer(level.GetMapLayer());
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
		// Convert to seconds.
		deltaTime *= 0.001;
		deltaTime = Clamp(deltaTime, 0.0f, 0.02f);

		// Move player. Trigger new GlowOrb creation.
		player.Update(deltaTime);

		// Update GlowOrbs (destroy old, create new, update sizes).
		level.Update();

		// Decide what has run into what.
		collision_manager.UpdateCollisions();

		// Show level.
		viewport.Draw(screen);

		// Show player.
		player.Draw(screen);
	}


	// -----------------------------------------------------------
	// Key events
	// -----------------------------------------------------------

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