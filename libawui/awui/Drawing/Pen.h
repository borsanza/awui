// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPEN_H__
#define __AWUIPEN_H__

#include <awui/Object.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Drawing2D/LineCap.h>
#include <awui/Drawing/Drawing2D/LineJoin.h>

namespace awui {
	namespace Drawing {
		class Pen : public Object {
			private:
				awui::Drawing::Color color;
				float width;
				Drawing2D::LineCap::Enum lineCap;
				Drawing2D::LineJoin::Enum lineJoin;

			public:
				Pen(awui::Drawing::Color color);
				Pen(awui::Drawing::Color color, float width);
				virtual ~Pen();

				virtual int IsClass(Classes::Enum objectClass) const;

				awui::Drawing::Color GetColor();
				void SetColor(awui::Drawing::Color color);

				float GetWidth();
				void SetWidth(float width);

				void SetLineJoin(Drawing2D::LineJoin::Enum lineJoin);
				Drawing2D::LineJoin::Enum GetLineJoin();

				void SetLineCap(Drawing2D::LineCap::Enum lineCap);
				Drawing2D::LineCap::Enum GetLineCap();
		};
	}
}

#endif
