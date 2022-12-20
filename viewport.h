#pragma once

#include "collidable.h"

namespace Tmpl8
{
	class Viewport : public Collidable
	{
	public:
		Viewport();

		void ProcessCollisions() {};
	};
};

