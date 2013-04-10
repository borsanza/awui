// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Collections/ArrayList.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Control;

			class ControlCollection : public Collections::ArrayList {
			private:
				Control * owner;

			public:
				ControlCollection(Control *);
				~ControlCollection();

				virtual int IsClass(Classes::Enum objectClass) const;

				Control * GetOwner();
				virtual void Add(void * item);
			};
		}
	}
}
