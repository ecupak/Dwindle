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


	private:
		void UpdatePosition();

		Camera& m_camera;
		std::vector<Collidable*> m_collisions;
	};
};

