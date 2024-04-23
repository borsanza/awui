#pragma once

#include <awui/Core/ColorF.h>
#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms::Station {
	class Gradient : public Control {
	  private:
		ColorF m_color[4];
		ColorF m_colorGo[4];
		ColorF InterpolateColor(ColorF *c1, ColorF *c2, float percent);

	  public:
		Gradient();
		virtual ~Gradient() = default;

		virtual bool IsClass(Classes objectClass) const override;

		void SetColor(int pos, const ColorF color);
		void SetColorGo(int pos, const ColorF color);

		virtual void OnPaint(OpenGL::GL *gl);
		virtual void OnTick(float deltaSeconds);
	};
} // namespace awui::Windows::Forms::Station
