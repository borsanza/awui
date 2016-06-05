#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Heartbeat : public Control {
					private:
						bool heartbeat;

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
