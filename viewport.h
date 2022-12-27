#pragma once

#include <vector>

#include "camera.h"
#include "life_hud.h"


namespace Tmpl8
{
	class Viewport
	{
	public:
		Viewport(Camera& camera);
		void Update(float deltaTime);
		void Draw(Surface* visible_layer);

		Camera& GetCamera();
		LifeHUD& GetLifeHUD();

	private:
		Camera& m_camera;
		LifeHUD m_life_hud;
	};
};

