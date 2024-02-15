#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form;

			class Application : public Object {
				static int quit;

				private:
					static void ProcessEvents(Form * form);

				public:
					static void Run(Form * form);

					virtual bool IsClass(Classes objectClass) const override;

					static void Quit();
			};
		}
	}
}
