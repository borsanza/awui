// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Spinner : public Control {
					private:
						int position;
						void DrawLine(int x, int y, int x2, int y2);

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
