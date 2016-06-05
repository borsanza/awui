#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Spinner : public Control {
					private:
						int position;

					public:
						Spinner();
						virtual ~Spinner();

						virtual void OnTick();
						virtual void OnPaint(OpenGL::GL * gl);
				};
			}
		}
	}
}
