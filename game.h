#pragma once

#include "key_state.h"
#include "template.h"
#include "level.h"

namespace Tmpl8 {

class Surface;
class Game
{
public:
	Game::Game(Surface* surface);
	//void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();	
	void Tick( float deltaTime );
	void MouseUp(int button) {};
	void MouseDown(int button) {};
	void MouseMove(int x, int y) {};
	void KeyUp(int key);
	void KeyDown(int key);

private:
	void Game::BuildNextLevel();

	// ATTRIBUTES
	Surface* screen{ nullptr };
	keyState leftKey;
	keyState rightKey;
	keyState upKey;
	keyState downKey;

	Level current_level;
	int level_id{ 1 };

	bool startloop{ false };
	int loop_switch{ -1 };
};

}; // namespace Tmpl8