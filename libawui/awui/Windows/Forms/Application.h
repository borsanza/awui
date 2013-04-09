// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _ag ag;

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form;

			class Application : public Object {
				friend class Form;

			private:
				static ag * g;

			public:
				static void Run(Form * form);

				virtual int IsClass(awui::Classes objectClass) const;
			};
		}
	}
}
