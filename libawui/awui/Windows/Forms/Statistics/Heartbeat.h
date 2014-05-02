// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Heartbeat : public Control {
					private:
						int heartbeat;

					public:
						Heartbeat();
						virtual ~Heartbeat();

						virtual void OnPaint(OpenGL::GL * gl);
						virtual void OnRemoteHeartbeat();
				};
			}
		}
	}
}
