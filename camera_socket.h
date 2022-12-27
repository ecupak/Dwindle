#pragma once

#include "socket.h"
#include "template.h"

namespace Tmpl8
{
	enum class Location
	{
		GROUND,
		WALL,
		OTHER,
	};


	struct CameraMessage
	{
	public:
		CameraMessage(vec2 new_center, Location new_location) :
			m_new_center{ new_center },
			m_new_location{ new_location }
		{	}

		vec2 m_new_center;
		Location m_new_location;
	};
};