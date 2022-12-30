#include "viewport.h"

#include "template.h"


namespace Tmpl8
{
	Viewport::Viewport(Surface* visible_layer, Camera& camera) :
		m_visible_layer{ visible_layer },
		m_camera{ camera }
	{	}

	
	Camera& Viewport::GetCamera()
	{
		return m_camera;
	}


	Socket<ViewportMessage>* Viewport::GetViewportSocket()
	{
		return &m_viewport_hub;
	}


	Socket<LifeMessage>* Viewport::GetLifeHUDSocket()
	{
		return m_life_hud.GetLifeHUDSocket();
	}


	void Viewport::Update(float deltaTime)
	{
		m_camera.Update(deltaTime);
		m_life_hud.Update(deltaTime);
	}


	void Viewport::CheckSocketForNewViewportMessage()
	{
		if (m_viewport_hub.HasNewMessage())
		{
			ProcessMessages();
		}
	}


	void Viewport::ProcessMessages()
	{
		std::vector<ViewportMessage> messages = m_viewport_hub.ReadMessages();
		m_viewport_hub.ClearMessages();

		for (ViewportMessage& message : messages)
		{
			switch (message.m_action)
			{
			case ViewportAction::MAP_SURFACE:
				//m_map_layer = &(message.m_layer);
				break;
			case ViewportAction::OBSTACLE_SURFACE:
				//m_obstacle_layer = &(message.m_layer);
				break;
			}
		}
	}


	void Viewport::Draw()
	{		
		m_visible_layer->Clear(0x00000000);
		m_camera.Draw(m_visible_layer);
		m_life_hud.Draw(m_visible_layer);
	}
};
