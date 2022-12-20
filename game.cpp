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
		level{ 1 },
		player{ surface, level.m_player_start_position },
		viewport{},
		collision_manager{ viewport, player }
	{	}

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

	// -----------------------------------------------------------
	// Initialize the application
	// -----------------------------------------------------------
	void Game::Init()
	{
		PrepareNextLevel();
	}

	void Game::PrepareNextLevel()
	{
		//BuildLevel();
		//SetPlayerStartPosition(); // may not need
		RegisterLevelCollidables();
	}

	void Game::BuildLevel()
	{
		level_id = 1;
		level.CreateLevel(level_id); // starts at 1.
	}

	void Game::SetPlayerStartPosition()
	{
		player.SetPosition(level.m_player_start_position);
	}

	void Game::RegisterLevelCollidables()
	{
		collision_manager.SetNewLevel(level);
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

		player.Update(deltaTime, leftKey, rightKey, upKey, downKey);
		//level.Update(deltaTime, leftKey, rightKey, upKey, downKey);
		//level.CollisionLoop();
		collision_manager.UpdateCollisions();

		level.Draw(screen);
		player.Draw(screen);
	}
};