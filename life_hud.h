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
		void LifeHUD::DrawIcon(Surface* visible_layer);

	private:
		int GetSurfaceWidth(int word_length);
		int GetSurfaceHeight();
		void InitializeDrawingValues();
		void UpdateValueLayer(LifeMessage& message);
		char*  ConvertInt2CharPointer(int new_value);

		Socket<LifeMessage> m_life_hub;

		char* m_value_as_char{ "00" };

		std::string m_value{ "00" };
		std::string m_tutorial_message{ "Tutorial mode 1-UP" };

		Surface m_value_layer;
		Sprite m_value_sprite;
		Sprite m_heart_sprite;

		Surface m_icon_layer;
		Surface m_tutorial_message_layer;
		Sprite m_tutorial_message_sprite;

		float m_opacity{ 1.0f };
		float m_flash_opacity{ 1.0f };
		float m_tutorial_message_opacity{ 0.0f };
			
		int m_value_scaling_factor = 5;
		int m_tutorial_message_scaling_factor = 2;

		int m_value_scaled_width{ 0 };
		int m_value_scaled_height{ 0 };		
		int m_tutorial_message_scaled_width{ 0 };
		int m_tutorial_message_scaled_height{ 0 };
				
		int m_vertical_box_padding = 3;
		int m_horizontal_box_padding = 5;
		int m_spacing = 10; // gap between heart and values.
		int m_control_spacing = 16; // gap between life box and tutorial message;

		vec2 m_box_start{ 0.0f, 0.0f };
		vec2 m_icon_start{ 0.0f, 0.0f };
		vec2 m_value_start{ 0.0f, 0.0f };
		vec2 m_box_end{ 0.0f, 0.0f };

		vec2 m_tutorial_message_start{ 0.0f, 0.0f };
		vec2 m_tutorial_message_end{ 0.0f, 0.0f };

		unsigned int m_draw_color{ 0xFFFFFFFF };
	};
};

