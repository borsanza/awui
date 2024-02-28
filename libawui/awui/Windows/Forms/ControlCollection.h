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

					virtual bool IsClass(Classes objectClass) const override;

					Control * GetOwner();
					virtual void Add(Control * item, bool fixSelected = false);
					virtual void Remove(Control * item);

					void MoveToEnd(Control * item);

					void Replace(Object * oldItem, Object * newItem) override;
			};
		}
	}
}
