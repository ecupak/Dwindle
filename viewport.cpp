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
	

	void Viewport::UpdatePosition()
	{
		left = 0;
		right = ScreenWidth - 1;
		top = 0;
		bottom = ScreenHeight - 1;
	}
};
