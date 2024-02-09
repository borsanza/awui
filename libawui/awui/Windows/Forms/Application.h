#pragma once

#include <awui/Object.h>

typedef struct _SDL_Joystick SDL_Joystick;

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form;

			class Application : public Object {
				static int quit;
				static SDL_Joystick *stick1;
				static SDL_Joystick *stick2;

				public:
					static void Run(Form * form);

					virtual bool IsClass(Classes objectClass) const override;

					static void Quit();
			};
		}
	}
}
