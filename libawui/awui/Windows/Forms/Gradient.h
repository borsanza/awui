#pragma once

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				class Gradient : public Control {
/*
					public:
						struct Orientation {
							enum Enum {
								Horizontal = 0,
								Vertical = 1,
							};
						};
*/
					private:
						// Orientation::Enum _orientation;
						Drawing::Color _color1;
						Drawing::Color _color2;
						Drawing::Color _color3;
						Drawing::Color _color4;

					public:
						Gradient();
						virtual ~Gradient();

						virtual int IsClass(Classes::Enum objectClass) const;

						virtual void SetColor1(const Drawing::Color color);
						virtual void SetColor2(const Drawing::Color color);
						virtual void SetColor3(const Drawing::Color color);
						virtual void SetColor4(const Drawing::Color color);

						// Gradient::Orientation::Enum GetOrientation();
						// void SetOrientation(Gradient::Orientation::Enum orientation);

						virtual void OnPaint(OpenGL::GL* gl);
				};
			}
		}
	}
}
