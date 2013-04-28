// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Stats {
					public:
						Stats();
						virtual ~Stats();

						void SetTimeBeforeVSync();
						void SetTimeAfterVSync();

						void Draw(OpenGL::GL* gl, int width, int height);
				};
			}
		}
	}
}
