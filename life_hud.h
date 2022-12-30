#pragma once

#include <string>

#include "surface.h"
#include "template.h"
#include "life_socket.h"


namespace Tmpl8
{
	class LifeHUD
	{
	public:
		LifeHUD();
		Socket<LifeMessage>* GetLifeHUDSocket();

		void Update(float deltaTime);
		void Draw(Surface* visible_layer);

	private:
		int GetSurfaceWidth();
		int GetSurfaceHeight();
		int GetWordLength(std::string word);
		void InitializeDrawingValues();
		void UpdateValueLayer(int new_value, float new_opacity);
		char*  ConvertInt2CharPointer(int new_value);

		Socket<LifeMessage> m_life_hub;

		Surface m_value_layer;
		Sprite m_value_sprite;
		Sprite m_heart_sprite;

		float m_opacity{ 1.0f };
		float m_flash_opacity{ 1.0f };

		char* m_value_as_char{ "00" };

		std::string m_value{ "00" };

		int m_scaling_factor = 5;

		int m_calculated_scaled_height{ 0 };
		int m_drawn_scaled_height{ 0 };
		int m_scaled_width{ 0 };

		int m_height{ 0 };

		int m_vertical_box_padding = 3;
		int m_horizontal_box_padding = 5;
		int m_spacing = 10; // gap between heart and values.

		int m_heart_padding{ 0 };
		int m_value_padding{ 0 };

		vec2 m_box_start{ 0.0f, 0.0f };
		vec2 m_heart_start{ 0.0f, 0.0f };
		vec2 m_value_start{ 0.0f, 0.0f };
		vec2 m_box_end{ 0.0f, 0.0f };

		unsigned int m_draw_color{ 0xFFFFFFFF };
	};
};

