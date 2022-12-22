#include "viewport.h"

#include "template.h"


namespace Tmpl8
{
	Viewport::Viewport(Camera& camera) :
		m_camera{ camera }
	{
		left = 0;
		right = ScreenWidth - 1;
		top = 0;
		bottom = ScreenHeight - 1;
	}


	void Viewport::SetBackgroundLayer(Surface* surface)
	{
		m_background_layer = surface;
	}
	
	
	void Viewport::SetObstacleLayer(Surface* surface)
	{
		m_obstacle_layer = surface;
	}
	
	
	void Viewport::SetMapLayer(Surface* surface)
	{
		m_map_layer = surface;
	}


	void Viewport::ResolveCollision(Collidable*& collision)
	{
		m_collisions.push_back(collision);
	}


	void Viewport::ProcessCollisions()
	{
		// If player has hit edge of camera center_box, move viewport to follow.
		// viewport does not move beyond edge of level bounds.
	}


	void Viewport::Draw(Surface* screen)
	{
		screen->Clear(0x00000000);
		//m_obstacle_layer->CopyTo(screen, 0, 0);

		for (Collidable*& collision : m_collisions)
		{
			switch (collision->m_object_type)
			{
			case CollidableType::FULL_GLOW:
				collision->Draw(screen, m_map_layer, left, top, right, bottom);
				break;
			case CollidableType::TEMP_GLOW:
				collision->Draw(screen, m_map_layer, left, top, right, bottom);
				break;
			case CollidableType::SAFE_GLOW:
				collision->Draw(screen, m_obstacle_layer, left, top, right, bottom);
				break;
			}
		}

		m_collisions.clear();

		m_camera.Draw(screen);

		if (m_camera.HasMoved())
		{
			//UpdatePosition();
		}
	}


	void Viewport::UpdatePosition()
	{
		left = 0;
		right = ScreenWidth - 1;
		top = 0;
		bottom = ScreenHeight - 1;
	}
};
