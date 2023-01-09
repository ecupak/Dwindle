#include "viewport.h"

#include "template.h"


namespace Tmpl8
{
	Viewport::Viewport(Surface* visible_layer, Camera& camera, GameMode& game_mode, MouseManager& mouse_manager) :
		m_visible_layer{ visible_layer },
		m_game_mode{ game_mode },
		m_camera{ camera },
		m_mouse_manager{ mouse_manager }
	{	}


	void Viewport::SetGameSocket(Socket<GameMessage>* game_socket)
	{
		m_game_socket = game_socket;
	}


	void Viewport::CreateButtons()
	{
		m_buttons.reserve(2);
		CreateButton(ButtonType::TUTORIAL);
		CreateButton(ButtonType::GAME);
	}


	void Viewport::CreateButton(ButtonType button_type)
	{
		switch (button_type)
		{
		case ButtonType::TUTORIAL:
			{
				std::function<void()> onclick{
				[&]() {m_game_socket->SendMessage(GameMessage{ GameAction::START_TUTORIAL });}
				};

				m_buttons.emplace_back(
					"Tutorial",
					5.0f,
					vec2{ 100.0f, 66.0f },
					vec2{ 300.0f, 100.0f },
					onclick,
					m_mouse_manager
				);
			}
			break;
		case ButtonType::GAME:
			{
				std::function<void()> onclick{
				[&]() {m_game_socket->SendMessage(GameMessage{ GameAction::START_GAME });}
				};

				m_buttons.emplace_back(
					"Start",
					5.0f,
					vec2{ 100.0f, 232.0f },
					vec2{ 300.0f, 100.0f },
					onclick,
					m_mouse_manager
				);
			}
			break;
		default:
			break;
		}
	}


	Camera& Viewport::GetCamera()
	{
		return m_camera;
	}


	Socket<LifeMessage>* Viewport::GetLifeHUDSocket()
	{
		return m_life_hud.GetLifeHUDSocket();
	}


	void Viewport::Update(float deltaTime)
	{
		m_camera.Update(deltaTime);

		if (m_game_mode == GameMode::GAME_SCREEN)
		{			
			m_life_hud.Update(deltaTime);
		}
		else if (m_game_mode == GameMode::TITLE_SCREEN)
		{
			for (Button& button : m_buttons)
			{				
				if (button.Update())
				{
					m_is_selection_made = true;
				}
			}
		}
	}


	void Viewport::Draw(float deltaTime)
	{		
		m_visible_layer->Clear(0x00000000);		

		if (m_game_mode == GameMode::GAME_SCREEN)
		{
			m_camera.Draw(m_visible_layer);
			m_life_hud.Draw(m_visible_layer);
		}
		else if (m_game_mode == GameMode::TITLE_SCREEN)
		{
			if (!m_is_selection_made)
			{
				for (Button& button : m_buttons)
				{
					button.Draw(m_visible_layer);
				}
			}
			m_camera.Draw(m_visible_layer);
		}
		else if (m_game_mode == GameMode::GAME_OVER)
		{
			m_camera.Draw(m_visible_layer);
		}
	}
};
