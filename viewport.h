#pragma once

#include "collidable.h"

namespace Tmpl8
{
	class Viewport : public Collidable
	{
	public:
		Viewport();

		void ProcessCollisions();
		virtual void ResolveCollision(Collidable*& collision);

		void SetBackgroundLayer(Surface* surface);
		void SetObstacleLayer(Surface* surface);
		void SetMapLayer(Surface* surface);

		virtual void Draw(Surface* screen);

	private:
		Surface* m_background_layer{ nullptr };
		Surface* m_obstacle_layer{ nullptr };
		Surface* m_map_layer{ nullptr };

		std::vector<Collidable*> m_collisions;
	};
};

