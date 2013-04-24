// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form;

			class Application : public Object {
				static int quit;

				public:
					static void Run(Form * form);

					virtual int IsClass(Classes::Enum objectClass) const;

					static void Quit();
			};
		}
	}
}
