#pragma once

#include <vector>

#include "surface.h"
#include "collidable.h"

namespace Tmpl8
{

	class Obstacle: public Collidable
	{
	public:		
		Obstacle(int x, int y, int tile_size, CollidableType object_type, Surface* autotile_map, int autotile_max_frame_id, int autotile_frame_id);
		virtual void ResolveCollision(Collidable*& collision);
		virtual void ResolveCollision(std::vector<Collidable*> collisions);
		virtual void Draw(Surface* screen);

	private:
		Sprite m_sprite;
		int m_autotile_id;

	};

};
