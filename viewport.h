#pragma once

#include <vector>

#include "viewport_socket.h"
#include "mouse_manager.h"

#include "camera.h"
#include "life_hud.h"
#include "button.h"
#include "game_enums.h"
#include "game_socket.h"


namespace Tmpl8
{	
	enum class ButtonType
	{
		TUTORIAL,
		GAME,
	};

	class Viewport
	{
	public:
		Viewport(Surface* visible_layer, Camera& camera, GameMode& game_mode, MouseManager& mouse_manager);
		
		void SetGameSocket(Socket<GameMessage>* game_socket);
		void CreateButtons();

		void Update(float deltaTime);
		void Draw(float detlaTime);

		Camera& GetCamera();
		Socket<LifeMessage>* GetLifeHUDSocket();

	private:		
		void CreateButton(ButtonType button_type);
		Button CreateTutorialButton();
		Button CreateGameButton();
		void CheckSocketForNewViewportMessage();
		void ProcessMessages();
		
		Socket<GameMessage>* m_game_socket{ nullptr };
		GameMode& m_game_mode;
		Camera& m_camera;
		MouseManager& m_mouse_manager;

		LifeHUD m_life_hud;
		std::vector<Button> m_buttons;
		Surface* m_visible_layer;

		bool m_is_selection_made{ false };
	};

};

