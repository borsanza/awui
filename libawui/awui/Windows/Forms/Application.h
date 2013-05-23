// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _SDL_Joystick SDL_Joystick;

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form;

			class Application : public Object {
				static int quit;
				static SDL_Joystick *stick;

				public:
					static void Run(Form * form);

					virtual int IsClass(Classes::Enum objectClass) const;

					static void Quit();
			};
		}
	}
}
