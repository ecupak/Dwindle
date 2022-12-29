#pragma once

#include "player_echo.h"
#include "second_echo.h"

namespace Tmpl8
{
	class FirstEcho : public PlayerEcho
	{
	public:
		FirstEcho(Sprite& player_sprite);

	protected:
		void UpdateEcho(Echo& echo) override;

		void DrawEcho(Surface* visible_layer, int camera_left, int camera_top) override;

	private:
		SecondEcho m_player_echo;
	};
};

