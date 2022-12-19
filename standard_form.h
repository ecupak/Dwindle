#pragma once

namespace Tmpl8
{
	struct StandardForm
	{
	public:
		StandardForm::StandardForm() {}
		StandardForm::StandardForm(float Ax, float By, float Cb) :
			A{ Ax }, B{ By }, C{ Cb }
		{	}

		void StandardForm::SetVars(float Ax, float By, float Cb)
		{
			A = Ax, B = By, C = Cb;
		}

		float A{ 0.0f };
		float B{ 0.0f };
		float C{ 0.0f };
	};
};