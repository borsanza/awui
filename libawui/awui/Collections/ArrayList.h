#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class ArrayList : public Object {
			private:
				Object ** _data;
				int _count;
				int _size;

			public:
				ArrayList();
				virtual ~ArrayList();

				virtual bool IsClass(Classes objectClass) const override;

				virtual String ToString() const override;

				virtual void Add(Object * item);
				inline int GetCount() const { return this->_count; }

				void Clear();
				Object * Get(int index);
				int IndexOf(Object * item);
				void Remove(Object * item);
				void RemoveAt(int index);
				void SetChildIndex(Object * item, int index);
				virtual void Replace(Object * oldItem, Object * newItem);
		};
	}
}
