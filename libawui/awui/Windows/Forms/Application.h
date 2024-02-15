#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Windows {
		namespace Forms {
			class Form;

			class Application : public Object {
				static int quit;
				static Collections::ArrayList * m_controllersList;

				private:
					static void ProcessEvents(Form * form);
					static int GetControlerOrder(int which);

				public:
					static void Run(Form * form);

					virtual bool IsClass(Classes objectClass) const override;

					static void Quit();
			};
		}
	}
}
