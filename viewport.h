#pragma once

#include <vector>
#include "collidable.h"
#include "camera.h"

namespace Tmpl8
{
	class Viewport : public Collidable
	{
	public:
		Viewport(Camera& camera);

		void ProcessCollisions();
		virtual void ResolveCollision(Collidable*& collision);

		void SetBackgroundLayer(Surface* surface);
		void SetObstacleLayer(Surface* surface);
		void SetMapLayer(Surface* surface);

		virtual void Draw(Surface* screen);

	private:
		void UpdatePosition();


		Surface* m_background_layer{ nullptr };
		Surface* m_obstacle_layer{ nullptr };
		Surface* m_map_layer{ nullptr };
		Camera& m_camera;
		std::vector<Collidable*> m_collisions;
	};
};

