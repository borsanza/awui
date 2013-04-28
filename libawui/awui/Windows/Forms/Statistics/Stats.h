// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>
#include <awui/DateTime.h>

namespace awui {
	class String;

	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Stats {
					private:
						DateTime afterSync;
						DateTime beforeSync;
						DateTime afterSync_last;
						DateTime beforeSync_last;

						void DrawString(String text, int x, int y);

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
