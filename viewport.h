#pragma once

#include <vector>

#include "camera.h"
#include "viewport_socket.h"
#include "life_hud.h"


namespace Tmpl8
{
	class Viewport
	{
	public:
		Viewport(Surface* visible_layer, Camera& camera);
		void Update(float deltaTime);
		void Draw();

		Camera& GetCamera();
		Socket<LifeMessage>* GetLifeHUDSocket();
		Socket<ViewportMessage>* GetViewportSocket();

	private:
		void CheckSocketForNewViewportMessage();
		void ProcessMessages();

		Camera& m_camera;
		LifeHUD m_life_hud;
		Socket<ViewportMessage> m_viewport_hub;

		Surface* m_visible_layer;
	};
};

