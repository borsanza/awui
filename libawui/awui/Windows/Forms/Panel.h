// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Panel : public Control {
			public:
				Panel();
				~Panel();

				virtual int IsClass(Classes objectClass) const;

				const virtual awui::Drawing::Size GetMinimumSize() const;
			};
		}
	}
}
