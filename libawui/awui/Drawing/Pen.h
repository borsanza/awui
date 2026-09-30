#pragma once

#include <awui/Drawing/Color.h>
#include <awui/Drawing/LineStyle.h>

namespace awui::Drawing {
	class Pen {
	  private:
		awui::Drawing::Color m_color;
		float m_width;
		LineCap m_lineCap;
		LineJoin m_lineJoin;

	  public:
		Pen(awui::Drawing::Color color);
		Pen(awui::Drawing::Color color, float width);
		~Pen() = default;

		awui::Drawing::Color GetColor() const;
		void SetColor(awui::Drawing::Color color);

		float GetWidth() const;
		void SetWidth(float width);

		void SetLineJoin(LineJoin lineJoin);
		LineJoin GetLineJoin() const;

		void SetLineCap(LineCap lineCap);
		LineCap GetLineCap() const;
	};
} // namespace awui::Drawing
