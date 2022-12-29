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
		player{ surface, leftKey, rightKey, upKey, downKey },
		camera{ player },
		viewport{ camera, player.GetStartingLife() },
		collision_manager{ player, camera }
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
		PrepareCamera();
	}


	void Game::PrepareLevel()
	{
		level_id = 3;
		level.CreateLevel(level_id); // starts at 1.
		level.RegisterCollisionSocket(collision_manager.GetLevelCollisionSocket());
		level.RegisterCollisionSocketToGlowManager(collision_manager.GetGlowCollisionSocket());
	}


	void Game::PreparePlayer()
	{
		player.SetPosition(level.GetPlayerStartPosition());
		player.RegisterGlowSocket(level.GetPlayerGlowSocket());
		player.RegisterCameraSocket(camera.GetPlayerCameraSocket());
		player.RegisterLifeSocket(viewport.GetLifeHUD().GetPlayerLifeSocket());
	}

	
	void Game::PrepareCollisionManager()
	{
		collision_manager.SetNewLevel(level);
	}


	void Game::PrepareCamera()
	{
		camera.SetLevelBounds(level.GetBounds());
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

		/*
			Move player and check for collisions.
			- Reposition if in obstacle.
			- Trigger creation of new glow orb.
			- Set new mode (ground/wall/ceiling).		
		*/
		player.Update(deltaTime);
		collision_manager.UpdateCollisions(CollidableGroup::PLAYER);

		// Update GlowOrbs (destroy old, create new, update sizes).
		level.Update(deltaTime);

		/*
			Follow player if moved too far from center.
			- Objects only drawn to screen if they collide with the viewport.
		*/
		viewport.Update(deltaTime);
		collision_manager.UpdateCollisions(CollidableGroup::CAMERA);

		/*
			1. Draw objects the camera sees (glow orbs and pickups).
			2. Draw player on top of everything else.
			3. Draw rest of game overlay (HUD, pause menu, etc).
		*/
		screen->Clear(0x00000000);

		/*
			when dead
			- start dead bouncing (keep going in direction. rebound off walls.)
			- darken glow orbs to 0 opacity (automatically deleted by glow manager).
			- remove collision detection after glow orbs go away.
			- resume regular bouncing (restore health).
			- place ball back at start.
			- resume collisions.
		*/		
		viewport.Draw(screen);
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