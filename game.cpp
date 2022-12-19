#include <cstdio> //printf
#include <vector>

#include "game.h"
#include "surface.h"
#include "level.h"

#define not !


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
		current_level{ screen }
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
			if (not leftKey.isPressed)
			{
				leftKey.isActive = true;
				leftKey.isPressed = true;
				rightKey.isActive = false;
			}
			break;
		case 79: // RIGHT.
			if (not rightKey.isPressed)
			{
				rightKey.isActive = true;
				rightKey.isPressed = true;
				leftKey.isActive = false;
			}
			break;
		case 82: // UP.
			if (not upKey.isPressed)
			{
				upKey.isActive = true;
				upKey.isPressed = true;
				downKey.isActive = false;
			}
			break;
		case 81: // DOWN.
			if (not downKey.isPressed)
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
			startloop = true;

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
		Surface new_window(400, 400);

		// Prepare next level.
		BuildNextLevel();
	}

	void Game::BuildNextLevel()
	{
		level_id = 1;
		current_level.CreateLevel(level_id); // starts at 1.
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
		// clear screen.
		// Convert to seconds.
		deltaTime *= 0.001;
		deltaTime = Clamp(deltaTime, 0.0f, 0.02f);

		if (true)
		{
			current_level.Update(deltaTime, leftKey, rightKey, upKey, downKey);
			current_level.CollisionLoop();

			/*switch (loop_switch)
			{
			case -1:
				current_level.Update(deltaTime, leftKey, rightKey, upKey, downKey);
				break;
			case 1:
				current_level.CollisionLoop();
				break;
			}
			loop_switch *= -1;*/
		}

		current_level.Draw();
	}
};