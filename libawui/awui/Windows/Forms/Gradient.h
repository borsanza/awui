#pragma once

#include <awui/Drawing/ColorF.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Station {
				class Gradient : public Control {
					private:
						Drawing::ColorF m_color[4];
						Drawing::ColorF m_colorGo[4];
						Drawing::ColorF InterpolateColor(Drawing::ColorF * c1, Drawing::ColorF * c2, float percent);

					public:
						Gradient();
						virtual ~Gradient() = default;

						virtual bool IsClass(Classes objectClass) const override;

						void SetColor(int pos, const Drawing::ColorF color);
						void SetColorGo(int pos, const Drawing::ColorF color);

						virtual void OnPaint(OpenGL::GL* gl);
						virtual void OnTick(float deltaSeconds);
				};
			}
		}
	}
}
