#pragma once

#include <awui/Core/Color.h>
#include <awui/Drawing/Drawing2D/LineCap.h>
#include <awui/Drawing/Drawing2D/LineJoin.h>
#include <awui/Object.h>

namespace awui::Drawing {
	class Pen : public Object {
	  private:
		awui::Color m_color;
		float m_width;
		Drawing2D::LineCap m_lineCap;
		Drawing2D::LineJoin m_lineJoin;

	  public:
		Pen(awui::Color color);
		Pen(awui::Color color, float width);
		virtual ~Pen() = default;

		virtual bool IsClass(Classes objectClass) const override;

		awui::Color GetColor();
		void SetColor(awui::Color color);

		float GetWidth();
		void SetWidth(float width);

		void SetLineJoin(Drawing2D::LineJoin lineJoin);
		Drawing2D::LineJoin GetLineJoin();

		void SetLineCap(Drawing2D::LineCap lineCap);
		Drawing2D::LineCap GetLineCap();
	};
} // namespace awui::Drawing
