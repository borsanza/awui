#pragma once

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
					virtual ~ControlCollection();

					virtual int IsClass(Classes::Enum objectClass) const;

					Control * GetOwner();
					virtual void Add(Control * item);
					virtual void Remove(Control * item);

					void MoveToEnd(Control * item);

					virtual void Replace(Control * oldItem, Control * newItem);
			};
		}
	}
}
