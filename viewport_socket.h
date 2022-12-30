#pragma once

#include "socket.h"
#include "surface.h"


namespace Tmpl8
{
	enum class ViewportAction
	{
		OBSTACLE_SURFACE,
		MAP_SURFACE,
	};


	struct ViewportMessage
	{
	public:
		ViewportMessage(ViewportAction action, Surface& layer) :
			m_action{ action },
			m_layer{ layer } // We are taking this from level and delivering ownership to viewport.
		{	}

		ViewportAction m_action;
		Surface& m_layer;
	};
};