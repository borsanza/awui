#ifndef _AWUI_WINDOWS_FORMS_APPLICATION_H
#define _AWUI_WINDOWS_FORMS_APPLICATION_H

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

#endif
