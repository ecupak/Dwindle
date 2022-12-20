#include "viewport.h"

#include "template.h"


namespace Tmpl8
{
	Viewport::Viewport()
	{
		left = 0;
		right = ScreenWidth - 1;
		top = 0;
		bottom = ScreenHeight - 1;
	}
};