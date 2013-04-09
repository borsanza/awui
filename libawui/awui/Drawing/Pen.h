// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIPEN_H__
#define __AWUIPEN_H__

#include <awui/Object.h>
#include <awui/Drawing/Color.h>

namespace awui {
	namespace Drawing {
		class awuiLineCap {
		public:
			enum LineCap {
				Butt = 0,
				Square = 1,
				Round = 2,
			};
		};

		class awuiLineJoin {
		public:
			enum LineJoin {
				Miter = 0,
				Bevel = 1,
				Round = 2,
			};
		};

		class Pen : public Object {
		private:
			awui::Drawing::Color color;
			float width;
			awuiLineCap::LineCap lineCap;
			awuiLineJoin::LineJoin lineJoin;

		public:
			Pen(awui::Drawing::Color color);
			Pen(awui::Drawing::Color color, float width);
			~Pen();

			virtual int IsClass(awui::Classes objectClass) const;

			awui::Drawing::Color GetColor();
			void SetColor(awui::Drawing::Color color);

			float GetWidth();
			void SetWidth(float width);

			void SetLineJoin(awuiLineJoin::LineJoin lineJoin);
			awuiLineJoin::LineJoin GetLineJoin();

			void SetLineCap(awuiLineCap::LineCap lineCap);
			awuiLineCap::LineCap GetLineCap();
		};
	}
}

#endif
