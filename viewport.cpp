#include "viewport.h"

#include "template.h"


namespace Tmpl8
{
	Viewport::Viewport(Camera& camera, int player_starting_life) :
		m_life_hud{ LifeHUD(player_starting_life) },
		m_camera{ camera }
	{	}

	
	Camera& Viewport::GetCamera()
	{
		return m_camera;
	}


	LifeHUD& Viewport::GetLifeHUD()
	{
		return m_life_hud;
	}


	void Viewport::Update(float deltaTime)
	{
		m_camera.Update(deltaTime);
		m_life_hud.Update(deltaTime);
	}


	void Viewport::Draw(Surface* visible_layer)
	{
		m_camera.Draw(visible_layer);
		m_life_hud.Draw(visible_layer);
	}
};
